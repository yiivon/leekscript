#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <bitset>
#include "Compiler.hpp"
#include "value/Function.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSMpz.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/value/LSClosure.hpp"
#include "../colors.h"
#include "../util/utf8.h"
#include "semantic/SemanticAnalyzer.hpp"
#include "llvm/IR/GlobalVariable.h"
#include "../type/Type.hpp"
#include "../vm/Program.hpp"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "resolver/File.hpp"
#include "semantic/FunctionVersion.hpp"
#include "../type/Function_type.hpp"
#include "../type/Function_object_type.hpp"
#include "semantic/Variable.hpp"

namespace ls {

llvm::orc::ThreadSafeContext Compiler::Ctx(llvm::make_unique<llvm::LLVMContext>());
llvm::IRBuilder<> Compiler::builder(*Ctx.getContext());

Compiler::value::value() : v(nullptr), t(Type::void_) {}

Compiler::Compiler(VM* vm) : vm(vm),
	TM(llvm::EngineBuilder().selectTarget()),
	DL(TM->createDataLayout()),
	ObjectLayer(ES, [this](llvm::orc::VModuleKey) {
		return llvm::orc::LegacyRTDyldObjectLinkingLayer::Resources {
			std::make_shared<llvm::SectionMemoryManager>(),
			createLegacyLookupResolver(ES, [this](const std::string& Name) -> llvm::JITSymbol {
				// std::cout << "Resolve symbol " << Name << std::endl;
				if (auto Sym = CompileLayer.findSymbol(Name, false)) {
					return Sym;
				} else if (auto Err = Sym.takeError()) {
					return std::move(Err);
				}
				auto s = this->vm->resolve_symbol(Name);
				if (s) {
					return llvm::JITSymbol((llvm::JITTargetAddress) s, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				}
				if (Name == "null") return llvm::JITSymbol((llvm::JITTargetAddress) LSNull::get(), llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				if (Name == "true") return llvm::JITSymbol((llvm::JITTargetAddress) LSBoolean::get(true), llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				if (Name == "false") return llvm::JITSymbol((llvm::JITTargetAddress) LSBoolean::get(false), llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				if (Name == "mpzc") return llvm::JITSymbol((llvm::JITTargetAddress) &this->vm->mpz_created, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				if (Name == "mpzd") return llvm::JITSymbol((llvm::JITTargetAddress) &this->vm->mpz_deleted, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));

				if (auto SymAddr = llvm::RTDyldMemoryManager::getSymbolAddressInProcess(Name)) {
					return llvm::JITSymbol(SymAddr, llvm::JITSymbolFlags::Exported);
				}
				return nullptr;
			},
			[](llvm::Error Err) {
				llvm::cantFail(std::move(Err), "lookupFlags failed");
			})
		};
	}),
	CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(*TM)),
	OptimizeLayer(CompileLayer, [this](std::unique_ptr<llvm::Module> M) {
		auto m = optimizeModule(std::move(M));
		if (this->export_bitcode) {
			std::error_code EC;
			llvm::raw_fd_ostream bitcode(m->getName().str() + ".bc", EC, llvm::sys::fs::F_None);
			llvm::WriteBitcodeToFile(*m, bitcode);
			bitcode.flush();
		}
		if (this->export_optimized_ir) {
			std::error_code EC2;
			llvm::raw_fd_ostream ir(m->getName().str() + "-opti.ll", EC2, llvm::sys::fs::F_None);
			m->print(ir, nullptr);
			ir.flush();
		}
		return m;
	}) {
		llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
	}

std::unique_ptr<llvm::Module> Compiler::optimizeModule(std::unique_ptr<llvm::Module> M) {
	// Create a function pass manager.
	auto FPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(M.get());
	// Add some optimizations.
	FPM->add(llvm::createBasicAAWrapperPass());
	FPM->add(llvm::createInstructionCombiningPass());
	FPM->add(llvm::createReassociatePass());
	FPM->add(llvm::createGVNPass());
	FPM->add(llvm::createCFGSimplificationPass());
	FPM->doInitialization();
	// Run the optimizations over all functions in the module being added to the JIT.
	for (auto &F : *M)
		FPM->run(F);
	return M;
}

llvm::orc::VModuleKey Compiler::addModule(std::unique_ptr<llvm::Module> M, bool optimize, bool export_bitcode, bool export_optimized_ir) {
	auto K = ES.allocateVModule();
	this->export_bitcode = export_bitcode;
	this->export_optimized_ir = export_optimized_ir;
	cantFail(OptimizeLayer.addModule(K, std::move(M)));
	return K;
}

void Compiler::init() {
	mappings.clear();
	global_strings.clear();
}
void Compiler::end() {}

Compiler::value Compiler::clone(Compiler::value v) const {
	if (v.t->fold()->must_manage_memory()) {
		if (v.t->reference) {
			v = insn_load(v);
		}
		return insn_call(v.t->fold()->add_temporary(), {v}, "Value.clone");
	}
	return v;
}
Compiler::value Compiler::new_null() const {
	return get_symbol("null", Type::null);
}
Compiler::value Compiler::new_bool(bool b) const {
	return {llvm::ConstantInt::get(getContext(), llvm::APInt(1, b, false)), Type::boolean};
}

Compiler::value Compiler::new_integer(int x) const {
	return {llvm::ConstantInt::get(getContext(), llvm::APInt(32, x, true)), Type::integer};
}

Compiler::value Compiler::new_real(double r) const {
	return {llvm::ConstantFP::get(getContext(), llvm::APFloat(r)), Type::real};
}

Compiler::value Compiler::new_long(long l) const {
	return {llvm::ConstantInt::get(getContext(), llvm::APInt(64, l, true)), Type::long_};
}

Compiler::value Compiler::new_mpz() const {
	auto r = create_entry("m", Type::tmp_mpz);
	increment_mpz_created();
	insn_call(Type::void_, {r}, "Number.mpz_init");
	return r;
}

Compiler::value Compiler::new_const_string(std::string s) const {
	auto i = global_strings.find(s);
	if (i != global_strings.end()) {
		return i->second;
	}
	Compiler::value str { builder.CreateGlobalStringPtr(s, "s"), Type::i8_ptr };
	((Compiler*) this)->global_strings.insert({ s, str });
	return str;
}
Compiler::value Compiler::new_null_pointer(const Type* type) const {
	assert(type->is_pointer());
	return { llvm::ConstantPointerNull::get((llvm::PointerType*) type->llvm(*this)), type };
}
Compiler::value Compiler::new_function(const Type* type) const {
	assert(dynamic_cast<const Function_type*>(type));
	return insn_call(Type::fun_object(type->return_type(), type->arguments()), {new_integer(0)}, "Function.new");
}
Compiler::value Compiler::new_function(Compiler::value fun) const {
	assert(fun.t->is_function() or fun.t->is_function_pointer());
	auto f = insn_convert(fun, Type::i8_ptr);
	return insn_call(Type::fun_object(fun.t->return_type(), fun.t->arguments()), {f}, "Function.new");
}
Compiler::value Compiler::new_function(std::string name, const Type* type) const {
	assert(dynamic_cast<const Function_type*>(type));
	auto ptr = get_symbol(name, type);
	auto fun = insn_convert(ptr, Type::i8_ptr);
	return insn_call(Type::fun_object(type->return_type(), type->arguments()), {fun}, "Function.new");
}
Compiler::value Compiler::new_closure(Compiler::value f, std::vector<Compiler::value> captures) const {
	// std::cout << "new_closure " << captures << std::endl;
	auto fun = insn_convert(f, Type::i8_ptr);
	auto closure = insn_call(Type::closure(f.t->return_type(), f.t->arguments()), {fun}, "Function.new.1");
	for (const auto& capture : captures) {
		function_add_capture(closure, capture);
	}
	return closure;
}
Compiler::value Compiler::new_class(std::string name) const {
	return insn_call(Type::clazz(), {new_const_string(name)}, "Class.new");
}

Compiler::value Compiler::new_object() const {
	return insn_call(Type::tmp_object, {}, "Object.new");
}

Compiler::value Compiler::new_object_class(Compiler::value clazz) const {
	return insn_call(Type::tmp_any, {clazz}, "Object.new.1");
}

Compiler::value Compiler::new_set() const {
	return insn_call(Type::tmp_set(), {}, "Set.new");
}

Compiler::value Compiler::new_array(const Type* element_type, std::vector<Compiler::value> elements) const {
	auto folded_type = element_type->fold();
	auto array_type = Type::tmp_array(element_type);
	auto array = [&]() { if (folded_type->is_bool()) {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.0");
	} else if (folded_type->is_integer()) {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.1");
	} else if (folded_type->is_real()) {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.2");
	} else {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.3");
	}}();
	for (const auto& element : elements) {
		auto v = insn_move(insn_convert(element, folded_type));
		insn_push_array(array, v);
	}
	// size of the array + 1 operations
	inc_ops(elements.size() + 1);
	return array;
}

Compiler::value Compiler::create_entry(const std::string& name, const Type* type) const {
	// std::cout << "create_entry " << type << std::endl;
	return { CreateEntryBlockAlloca(name, type->llvm((Compiler&) *this)), type->pointer() };
}

Compiler::value Compiler::get_symbol(const std::string& name, const Type* type) const {
	// std::cout << "get_symbol(" << name << ", " << type << ")" << std::endl;
	if (dynamic_cast<const Function_type*>(type)) {
		auto fun = llvm::Function::Create((llvm::FunctionType*) type->llvm(*this), llvm::Function::ExternalLinkage, name, program->module);
		return { fun, type };
	} else {
		assert(type->is_pointer());
		Compiler::value ptr = { program->module->getGlobalVariable(name), type };
		if (!ptr.v) {
			auto t = type->llvm(*this)->getPointerElementType();
			ptr.v = new llvm::GlobalVariable(*program->module, t, false, llvm::GlobalValue::ExternalLinkage, nullptr, name);
		}
		assert(ptr.v->getType() == type->llvm(*this));
		return ptr;
	}
}

Compiler::value Compiler::to_int(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_integer()) {
		return v;
	}
	if (v.t->is_mpz_ptr()) {
		return to_int(insn_call(Type::long_, {v}, "Number.mpz_get_si"));
	}
	if (v.t->is_long() or v.t->is_bool()) {
		return { builder.CreateIntCast(v.v, Type::integer->llvm(*this), false), Type::integer };
	}
	if (v.t->is_real()) {
		return { builder.CreateFPToSI(v.v, Type::integer->llvm(*this)), Type::integer };
	}
	return insn_invoke(Type::integer, {v}, "Value.int");
}

Compiler::value Compiler::to_real(Compiler::value x, bool delete_temporary) const {
	assert(x.t->llvm(*this) == x.v->getType());
	if (x.t->is_real()) {
		return x;
	}
	if (x.t->is_bool()) {
		return { builder.CreateUIToFP(x.v, Type::real->llvm(*this)), Type::real };
	}
	if (x.t->is_long() or x.t->is_integer()) {
		return { builder.CreateSIToFP(x.v, Type::real->llvm(*this)), Type::real };
	}
	if (delete_temporary) {
		return insn_invoke(Type::real, {x}, "Value.real_delete");
	} else {
		return insn_invoke(Type::real, {x}, "Value.real");
	}
}

Compiler::value Compiler::to_long(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_long()) {
		return v;
	}
	if (v.t->is_bool() or v.t->is_integer()) {
		return { builder.CreateIntCast(v.v, Type::long_->llvm(*this), false), Type::long_ };
	}
	if (v.t->is_real()) {
		return { builder.CreateFPToSI(v.v, Type::long_->llvm(*this)), Type::long_ };
	}
	return insn_invoke(Type::long_, {v}, "Value.long");
}

Compiler::value Compiler::insn_convert(Compiler::value v, const Type* t, bool delete_previous) const {
	// std::cout << "convert " << v.t << " " << t->is_primitive() << " to " << t << " " << t->is_polymorphic() << std::endl;
	// assert(v.t->llvm(*this) == v.v->getType());
	if (!v.v) { return v; }
	if (v.t->not_temporary() == t) return v;
	if (v.t->is_function()) {
		if (t == Type::i8_ptr) {
			return { builder.CreatePointerCast(v.v, t->llvm(*this)), t };
		} else if (t->is_function_object()) {
			auto f = insn_convert(v, Type::i8_ptr);
			return insn_call(Type::fun_object(v.t->return_type(), v.t->arguments()), {f}, "Function.new");
		}
	}
	if (v.t->is_function_pointer() and t->is_function_object()) {
		auto f = insn_convert(v, Type::i8_ptr);
		return insn_call(Type::fun_object(v.t->return_type(), v.t->arguments()), {f}, "Function.new");
	}
	if (v.t->is_array() and t->is_array()) {
		// std::cout << "Convert " << v.t << " to " << t << std::endl;
		if (t->element()->is_polymorphic()) {
			if (v.t->element() == Type::integer) {
				auto r = insn_call(t, {v}, "Array.int_to_any");
				if (delete_previous) insn_delete_temporary(v);
				if (delete_previous) insn_delete(v);
				return r;
			} else if (v.t->element() == Type::real) {
				auto r = insn_call(t, {v}, "Array.real_to_any");
				if (delete_previous) insn_delete_temporary(v);
				if (delete_previous) insn_delete(v);
				return r;
			} else if (v.t->element()->is_polymorphic() and v.t->element() != Type::never) {
				return { builder.CreatePointerCast(v.v, t->llvm(*this)), t };
			}
		} else if (t->element()->is_real()) {
			if (v.t->element() == Type::integer) {
				auto r = insn_call(t, {v}, "Array.int_to_real");
				if (delete_previous) insn_delete_temporary(v);
				if (delete_previous) insn_delete(v);
				return r;
			}
		} else if (t->element()->is_integer()) {
			if (v.t->element()->is_real() or v.t->element()->is_polymorphic()) {
				if (delete_previous) insn_delete(v);
				insn_delete_temporary(v);
				return new_array(Type::integer, {});
			}
		} else if (t->element()->is_bool()) {
			return new_array(Type::boolean, {});
		}
		if (v.t->element()->is_never()) {
			if (delete_previous) insn_delete(v);
			insn_delete_temporary(v);
			return new_array(t->element(), {});
		}
	}
	if (v.t->is_set() and t->is_set() and t->element()->is_polymorphic()) {
		if (v.t->element() == Type::integer) {
			auto r = insn_call(t, {v}, "Set.int_to_any");
			if (delete_previous) insn_delete(v);
			return r;
		} else if (v.t->element() == Type::real) {
			auto r = insn_call(t, {v}, "Set.real_to_any");
			if (delete_previous) insn_delete(v);
			return r;
		}
	}
	if (v.t->fold()->is_polymorphic()) {
		if (t->is_polymorphic() or t->is_pointer()) {
			auto new_type = t;
			if (v.t->temporary) new_type = new_type->add_temporary();
			return { builder.CreatePointerCast(v.v, new_type->llvm(*this)), new_type };
		}
	} else if (t->is_polymorphic()) {
		return insn_to_any(v);
	}
	if (t->is_real()) {
		return to_real(v, delete_previous);
	} else if (t->is_integer()) {
		return to_int(v);
	} else if (t->is_long()) {
		return to_long(v);
	}
	return v;
}

Compiler::value Compiler::to_numeric(Compiler::value v) const {
	if (v.t->is_primitive()) return v;
	if (v.t->is_any()) {
		return insn_invoke(Type::real, {v}, "Value.real");
	}
	insn_throw_object(ls::vm::Exception::WRONG_ARGUMENT_TYPE);
	assert(false);
}

// Operators wrapping
Compiler::value Compiler::insn_not(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	return {builder.CreateNot(to_int(v).v), Type::integer};
}

Compiler::value Compiler::insn_not_bool(Compiler::value v) const {
	return { builder.CreateNot(insn_to_bool(v).v), Type::boolean };
}

Compiler::value Compiler::insn_neg(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_real()) {
		return {builder.CreateFNeg(v.v), v.t};
	} else {
		return {builder.CreateNeg(v.v), v.t};
	}
}

Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::boolean};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::boolean};
}

Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_add(" << a.t << ", " << b.t << ")" << std::endl;
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	auto a_type = a.t->fold();
	auto b_type = b.t->fold();
	if (a_type->is_polymorphic() or b_type->is_polymorphic()) {
		return insn_invoke(Type::any, {insn_to_any(a), insn_to_any(b)}, "Value.operator+");
	} else if (a_type->is_real() or b_type->is_real()) {
		return {builder.CreateFAdd(to_real(a).v, to_real(b).v), Type::real};
	} else if (a_type->is_long() or b_type->is_long()) {
		return {builder.CreateAdd(to_long(a).v, to_long(b).v), Type::long_};
	} else {
		return {builder.CreateAdd(to_int(a).v, to_int(b).v), Type::integer};
	}
}

Compiler::value Compiler::insn_sub(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	auto a_type = a.t->fold();
	auto b_type = b.t->fold();
	if (a_type->is_polymorphic() or b_type->is_polymorphic()) {
		return insn_invoke(Type::any, {insn_to_any(a), insn_to_any(b)}, "Value.operator-");
	} else if (a_type->is_real() or b_type->is_real()) {
		return {builder.CreateFSub(to_real(a).v, to_real(b).v), Type::real};
	} else {
		return {builder.CreateSub(to_int(a).v, to_int(b).v), Type::integer};
	}
}

Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_eq " << a.t << " == " << b.t << std::endl;
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	auto a_type = a.t->fold();
	auto b_type = b.t->fold();
	if (a_type->is_polymorphic() or b_type->is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean, {ap, bp}, "Value.operator==");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	}
	if (a_type->is_pointer() or b_type->is_pointer()) {
		if (a_type->is_pointer() and b_type->is_pointer()) {
			return insn_pointer_eq(a, b);
		}
		return new_bool(false);
	} else if (a_type->is_mpz_ptr() and b_type->is_integer()) {
		auto r = insn_eq(insn_call(Type::boolean, {a, b}, "Number._mpz_cmp_si"), new_integer(0));
		insn_delete_temporary(a);
		return r;
	} else if (a_type->is_mpz_ptr() and b_type->is_mpz_ptr()) {
		auto r = insn_eq(insn_call(Type::boolean, {a, b}, "Number.mpz_cmp"), new_integer(0));
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		return r;
	} else if (a_type->is_real() or b_type->is_real()) {
		return {builder.CreateFCmpOEQ(to_real(a).v, to_real(b).v), Type::boolean};
	} else if (a_type->is_long() or b_type->is_long()) {
		return {builder.CreateICmpEQ(to_long(a).v, to_long(b).v), Type::boolean};
	} else {
		return {builder.CreateICmpEQ(to_int(a).v, to_int(b).v), Type::boolean};
	}
}

Compiler::value Compiler::insn_pointer_eq(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_pointer());
	assert(b.t->is_pointer());
	auto p1 = builder.CreatePointerCast(a.v, llvm::Type::getInt8PtrTy(getContext()));
	auto p2 = builder.CreatePointerCast(b.v, llvm::Type::getInt8PtrTy(getContext()));
	return { builder.CreateICmpEQ(p1, p2), Type::boolean };
}

Compiler::value Compiler::insn_ne(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	if (a.t->is_real() or b.t->is_real()) {
		return {builder.CreateFCmpONE(to_real(a).v, to_real(b).v), Type::boolean};
	} else if (a.t->is_long() or b.t->is_long()) {
		return {builder.CreateICmpNE(to_long(a).v, to_long(b).v), Type::boolean};
	} else {
		return {builder.CreateICmpNE(to_int(a).v, to_int(b).v), Type::boolean};
	}
}

Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_lt " << a.t << " < " << b.t << std::endl;
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	if (a.t->is_polymorphic() or b.t->is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean, {ap, bp}, "Value.operator<");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	}
	Compiler::value r;
	if (a.t->is_integer() and b.t->is_mpz_ptr()) {
		auto r = insn_gt(insn_call(Type::integer, {b, a}, "Number._mpz_cmp_si"), new_integer(0));
		insn_delete_temporary(b);
		return r;
	} else if (a.t->is_mpz_ptr() and b.t->is_integer()) {
		auto r = insn_lt(insn_call(Type::integer, {a, b}, "Number._mpz_cmp_si"), new_integer(0));
		insn_delete_temporary(a);
		return r;
	} else if (a.t->is_mpz_ptr() and b.t->is_mpz_ptr()) {
		auto res = insn_call(Type::integer, {a, b}, "Number.mpz_cmp");
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		return insn_lt(res, new_integer(0));
	} else if (a.t->is_real() || b.t->is_real()) {
		r = {builder.CreateFCmpOLT(to_real(a).v, to_real(b).v), Type::boolean};
	} else if (a.t->is_long() || b.t->is_long()) {
		r = {builder.CreateICmpSLT(to_long(a).v, to_long(b).v), Type::boolean};
	} else {
		r = {builder.CreateICmpSLT(a.v, b.v), Type::boolean};
	}
	return r;
}

Compiler::value Compiler::insn_le(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	Compiler::value r;
	if (a.t->is_polymorphic() or b.t->is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean, {ap, bp}, "Value.operator<=");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	} else if (a.t->is_real() || b.t->is_real()) {
		r = {builder.CreateFCmpOLE(to_real(a).v, to_real(b).v), Type::boolean};
	} else if (a.t->is_long() || b.t->is_long()) {
		r = {builder.CreateICmpSLE(to_long(a).v, to_long(b).v), Type::boolean};
	} else {
		r = {builder.CreateICmpSLE(a.v, b.v), Type::boolean};
	}
	return r;
}

Compiler::value Compiler::insn_gt(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	Compiler::value r;
	if (a.t->is_mpz_ptr() and b.t->is_integer()) {
		auto res = insn_call(Type::integer, {a, b}, "Number._mpz_cmp_si");
		insn_delete_temporary(a);
		return insn_gt(res, new_integer(0));
	} else if (a.t->is_integer() and b.t->is_mpz_ptr()) {
		auto res = insn_call(Type::integer, {b, a}, "Number._mpz_cmp_si");
		insn_delete_temporary(b);
		return insn_lt(res, new_integer(0));
	} else if (a.t->is_real() || b.t->is_real()) {
		r = {builder.CreateFCmpOGT(to_real(a).v, to_real(b).v), Type::boolean};
	} else if (a.t->is_long() || b.t->is_long()) {
		r = {builder.CreateICmpSGT(to_long(a).v, to_long(b).v), Type::boolean};
	} else {
		r = {builder.CreateICmpSGT(a.v, b.v), Type::boolean};
	}
	return r;
}

Compiler::value Compiler::insn_ge(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	if (a.t->is_polymorphic() or b.t->is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean, {ap, bp}, "Value.operator>=");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	}
	Compiler::value r;
	if (a.t->is_real() || b.t->is_real()) {
		r = {builder.CreateFCmpOGE(to_real(a).v, to_real(b).v), Type::boolean};
	} else if (a.t->is_long() || b.t->is_long()) {
		r = {builder.CreateICmpSGE(to_long(a).v, to_long(b).v), Type::boolean};
	} else {
		r = {builder.CreateICmpSGE(a.v, b.v), Type::boolean};
	}
	return r;
}

Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_mul " << a.t << " " << b.t << " " << b.v->getType() << std::endl;
	assert(check_value(a));
	assert(check_value(b));
	auto a_type = a.t->fold();
	auto b_type = b.t->fold();
	if (a_type->is_polymorphic() or b_type->is_polymorphic()) {
		return insn_invoke(Type::any, {insn_to_any(a), insn_to_any(b)}, "Value.operator*");
	} else if (a_type->is_real() or b_type->is_real()) {
		return {builder.CreateFMul(to_real(a).v, to_real(b).v), Type::real};
	} else if (a_type->is_long() or b_type->is_long()) {
		return {builder.CreateMul(to_long(a).v, to_long(b).v), Type::long_};
	}
	return {builder.CreateMul(to_int(a).v, to_int(b).v), Type::integer};
}

Compiler::value Compiler::insn_div(Compiler::value a, Compiler::value b) const {
	if (a.t->is_string() and b.t->is_string()) {
		return insn_call(Type::array(Type::string), {a, b}, "Value.operator/");
	}
	auto bv = to_real(b);
	insn_if(insn_eq(bv, new_integer(0)), [&]() {
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
	});
	value r = { builder.CreateFDiv(to_real(a).v, bv.v), Type::real };
	insn_delete_temporary(a);
	insn_delete_temporary(b);
	return r;
}

Compiler::value Compiler::insn_int_div(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_primitive() && b.t->is_primitive());
	if (a.t->is_long() or b.t->is_long()) {
		return {builder.CreateSDiv(to_long(a).v, to_long(b).v), Type::long_};
	}
	return {builder.CreateSDiv(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_bit_and(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_bit_or(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_bit_xor(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_primitive() && b.t->is_primitive());
	return {builder.CreateXor(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_shl(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_primitive() && b.t->is_primitive());
	return {builder.CreateShl(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_ashr(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_primitive() && b.t->is_primitive());
	return {builder.CreateAShr(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_lshr(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_primitive() && b.t->is_primitive());
	return {builder.CreateLShr(a.v, b.v), Type::integer};
}

Compiler::value Compiler::insn_mod(Compiler::value a, Compiler::value b) const {
	assert(check_value(a));
	assert(check_value(b));
	insn_if(insn_eq(to_real(b), new_real(0)), [&]() {
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
	});
	if (a.t->is_long() and b.t->is_long()) {
		return { builder.CreateSRem(a.v, b.v), Type::long_ };
	} else if (a.t->is_long() and b.t->is_integer()) {
		return { builder.CreateSRem(a.v, to_long(b).v), Type::long_ };
	} else if (a.t->is_integer() and b.t->is_long()) {
		return to_int({ builder.CreateSRem(to_long(a).v, to_long(b).v), Type::long_ });
	} else if (a.t->is_integer() and b.t->is_integer()) {
		return { builder.CreateSRem(a.v, b.v), Type::integer };
	} else {
		value r = { builder.CreateFRem(to_real(a).v, to_real(b).v), Type::real };
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		return r;
	}
}

Compiler::value Compiler::insn_double_mod(Compiler::value a, Compiler::value b) const {
	if (a.t->is_polymorphic() or b.t->is_polymorphic()) {
		return insn_invoke(Type::any, {insn_to_any(a), insn_to_any(b)}, "Value.operator%%");
	} else {
		return insn_mod(insn_add(insn_mod(a, b), b), b);
	}
}

Compiler::value Compiler::insn_cmpl(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	return {builder.CreateSub(to_int(insn_to_bool(a)).v, to_int(insn_to_bool(b)).v), Type::integer};
}

Compiler::value Compiler::insn_log(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::real, {x}, "Number.m_log");
	}
	if (x.t->is_long()) {
		return insn_call(Type::real, {x}, "Number.m_log.1");
	}
	return insn_call(Type::real, {x}, "Number.m_log.2");
}

Compiler::value Compiler::insn_log10(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::real, {x}, "Number.m_log10");
	} else if (x.t->is_long()) {
		return insn_call(Type::real, {x}, "Number.m_log10.1");
	} else {
		return insn_call(Type::real, {x}, "Number.m_log10.2");
	}
}

Compiler::value Compiler::insn_ceil(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) return x;
	return to_int(insn_call(Type::real, {x}, "Number.m_ceil"));
}

Compiler::value Compiler::insn_round(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) return x;
	return to_int(insn_call(Type::real, {x}, "Number.m_round"));
}

Compiler::value Compiler::insn_floor(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) return x;
	if (x.t->is_long()) return x;
	return to_long(insn_call(Type::real, {x}, "Number.m_floor"));
}

Compiler::value Compiler::insn_cos(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_cos");
	}
	return insn_call(Type::real, {x}, "Number.m_cos.2");
}

Compiler::value Compiler::insn_sin(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_sin");
	}
	return insn_call(Type::real, {x}, "Number.m_sin.2");
}

Compiler::value Compiler::insn_tan(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_tan");
	}
	return insn_call(Type::real, {x}, "Number.m_tan.2");
}

Compiler::value Compiler::insn_acos(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_acos");
	}
	return insn_call(Type::real, {x}, "Number.m_acos.2");
}

Compiler::value Compiler::insn_asin(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_asin");
	}
	return insn_call(Type::real, {x}, "Number.m_asin.2");
}

Compiler::value Compiler::insn_atan(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_atan");
	}
	return insn_call(Type::real, {x}, "Number.m_atan.2");
}

Compiler::value Compiler::insn_pow(Compiler::value a, Compiler::value b) const {
	assert(a.t->llvm(*this) == a.v->getType());
	assert(b.t->llvm(*this) == b.v->getType());
	assert(a.t->is_primitive() && b.t->is_primitive());
	Compiler::value r;
	if (a.t->is_real() or b.t->is_real()) {
		r = insn_call(Type::real, {to_real(a), to_real(b)}, "Number.powdd");
	} else if (a.t->is_long()) {
		r = to_long(insn_call(Type::real, {a, to_int(b)}, "Number.powli"));
	} else if (a.t->is_integer()) {
		r = to_int(insn_call(Type::real, {a, b}, "Number.powii"));
	}
	return r;
}

Compiler::value Compiler::insn_min(Compiler::value x, Compiler::value y) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(y.t->llvm(*this) == y.v->getType());
	assert(x.t->is_primitive() && y.t->is_primitive());
	if (x.t->is_integer() and y.t->is_integer()) {
		return insn_call(Type::integer, {x, y}, "Number.m_min");
	}
	if (x.t->is_long() and y.t->is_long()) {
		return insn_call(Type::long_, {x, y}, "Number.m_min.1");
	}
	return insn_call(Type::real, {to_real(x), to_real(y)}, "Number.m_min.2");
}

Compiler::value Compiler::insn_max(Compiler::value x, Compiler::value y) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(y.t->llvm(*this) == y.v->getType());
	assert(x.t->is_primitive() && y.t->is_primitive());
	if (x.t->is_integer() and y.t->is_integer()) {
		return insn_call(Type::integer, {x, y}, "Number.m_max");
	}
	if (x.t->is_long() and y.t->is_long()) {
		return insn_call(Type::long_, {x, y}, "Number.m_max.1");
	}
	return insn_call(Type::real, {to_real(x), to_real(y)}, "Number.m_max.2");
}

Compiler::value Compiler::insn_exp(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(x.t->is_primitive());
	if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_exp");
	}
	return insn_call(Type::real, {x}, "Number.m_exp.1");
}

Compiler::value Compiler::insn_atan2(Compiler::value x, Compiler::value y) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(y.t->llvm(*this) == y.v->getType());
	assert(x.t->is_primitive() && y.t->is_primitive());
	if (x.t->is_integer() && y.t->is_integer()) {
		return insn_call(Type::integer, {x, y}, "Number.m_atan2");
	}
	return insn_call(Type::real, {to_real(x), to_real(y)}, "Number.m_atan2.2");
}

Compiler::value Compiler::insn_abs(Compiler::value x) const {
	assert(x.t->llvm(*this) == x.v->getType());
	if (x.t->is_polymorphic()) {
		return insn_call(Type::real, {to_real(x)}, "Number.m_abs.2");
	} else if (x.t->is_integer()) {
		return insn_call(Type::integer, {x}, "Number.m_abs");
	} else if (x.t->is_long()) {
		return insn_call(Type::long_, {x}, "Number.m_abs.1");
	} else {
		return insn_call(Type::real, {x}, "Number.m_abs.2");
	}
}

Compiler::value Compiler::insn_to_any(Compiler::value v) const {
	// std::cout << "to_any " << v.t << std::endl;
	if (v.t->is_polymorphic()) {
		return v; // already any
	}
	if (v.t->is_long()) {
		return insn_call(Type::tmp_any, {to_real(v)}, "Number.new");
	} else if (v.t->is_real()) {
		return insn_call(Type::tmp_any, {v}, "Number.new");
	} else if (v.t->is_bool()) {
		if (auto* ci = llvm::dyn_cast<llvm::ConstantInt>(v.v)) {
			if (ci->equalsInt(0)) return get_symbol("false", Type::tmp_any);
			if (ci->equalsInt(1)) return get_symbol("true", Type::tmp_any);
		}
		return { builder.CreateSelect(v.v, get_symbol("true", Type::tmp_any).v, get_symbol("false", Type::tmp_any).v), Type::tmp_any };
	} else if (v.t->is_mpz_ptr()) {
		if (v.t->temporary) {
			return insn_call(Type::tmp_any, {insn_load(v)}, "Number.new.1");
		} else {
			return insn_call(Type::tmp_any, {insn_load(v)}, "Number.new.2");
		}
	} else if (v.t->is_function_pointer()) {
		return new_function(v);
	} else {
		return insn_call(Type::tmp_any, {to_real(v)}, "Number.new");
	}
}

Compiler::value Compiler::insn_to_bool(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_bool()) {
		return v;
	}
	if (v.t->is_integer() or v.t->is_long()) {
		return { builder.CreateICmpNE(v.v, llvm::Constant::getNullValue(v.v->getType())), Type::boolean };
	}
	if (v.t->is_real()) {
		return { builder.CreateFCmpONE(v.v, llvm::Constant::getNullValue(v.v->getType())), Type::boolean };
	}
	if (v.t->is_string()) {
		return insn_call(Type::boolean, {v}, "String.to_bool");
	}
	if (v.t->is_array()) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		return insn_call(Type::boolean, {v}, "Array.to_bool");
	}
	if (v.t->is_function() or v.t->is_function_pointer()) {
		return new_bool(true);
	}
	if (v.t->is_mpz_ptr()) {
		// TODO
		return v;
	}
	return insn_call(Type::boolean, {v}, "Value.to_bool");
}

Compiler::value Compiler::insn_load(Compiler::value v) const {
	// std::cout << "insn_load " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t->llvm(*this) == v.v->getType());
	assert(v.t->is_pointer());
	Compiler::value r { builder.CreateLoad(v.v), v.t->pointed() };
	assert(r.t == v.t->pointed());
	assert(r.t->llvm(*this) == r.v->getType());
	if (v.t->temporary) r.t = r.t->add_temporary();
	return r;
}
Compiler::value Compiler::insn_load_member(Compiler::value v, int pos) const {
	auto type = v.t->fold();
	assert(type->llvm(*this) == v.v->getType());
	assert(type->is_pointer());
	assert(type->pointed()->is_struct());
	auto s = builder.CreateStructGEP(type->pointed()->llvm(*this), v.v, pos);
	return { builder.CreateLoad(s), type->pointed()->member(pos) };
}

void Compiler::insn_store(Compiler::value x, Compiler::value y) const {
	// std::cout << "insn_store " << x.t << " " << x.v->getType() << " " << y.t << std::endl;
	assert(x.t->llvm(*this) == x.v->getType());
	// assert(y.t->llvm(*this) == y.v->getType());
	// std::cout << x.t->pointed()->fold() << " = " << y.t->fold()->not_temporary() << std::endl;
	// assert(x.t->pointed()->fold() == y.t->fold()->not_temporary());
	builder.CreateStore(y.v, x.v);
}
void Compiler::insn_store_member(Compiler::value x, int pos, Compiler::value y) const {
	assert(x.t->llvm(*this) == x.v->getType());
	assert(y.t->llvm(*this) == y.v->getType());
	assert(x.t->is_pointer());
	assert(x.t->pointed()->is_struct());
	assert(x.t->pointed()->member(pos) == y.t->not_temporary());
	auto s = builder.CreateStructGEP(x.t->pointed()->llvm(*this), x.v, pos);
	builder.CreateStore(y.v, s);
}

Compiler::value Compiler::insn_typeof(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->fold()->is_any()) {
		return insn_call(Type::integer, {v}, "Value.type");
	}
	return new_integer(v.t->id());
}

Compiler::value Compiler::insn_class_of(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	auto clazz = v.t->class_name();
	if (clazz.size() and clazz != "Value") {
		return get_symbol(clazz, Type::clazz(clazz));
	} else {
		return insn_call(Type::clazz(), {v}, "Value.get_class");
	}
}

void Compiler::insn_delete(Compiler::value v) const {
	// std::cout << "insn_delete " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->must_manage_memory()) {
		insn_call(Type::void_, {v}, "Value.delete_ref");
	} else if (v.t->is_mpz_ptr()) {
		insn_delete_mpz(v);
	}
}

void Compiler::insn_delete_variable(Compiler::value v) const {
	// std::cout << "delete variable " << v.t << std::endl;
	assert(check_value(v));
	if (v.t->is_mpz_ptr()) {
		insn_delete_mpz(v);
	} else {
		if (v.t->pointed()->must_manage_memory()) {
			insn_delete(insn_load(v));
		}
	}
}

void Compiler::insn_delete_temporary(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (not v.t->temporary) return;
	if (v.t == Type::tmp_mpz_ptr) {
		insn_delete_mpz(v);
	} if (v.t->must_manage_memory()) {
		insn_call(Type::void_, {v}, "Value.delete");
	}
}

Compiler::value Compiler::insn_array_size(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_string()) {
		return insn_call(Type::integer, {v}, "String.isize");
	} else if (v.t->is_array()) {
		if (v.t->element() == Type::never) {
			return new_integer(0);
		} else if (v.t->element()->is_integer()) {
			return insn_call(Type::integer, {v}, "Array.isize.2");
		} else if (v.t->element()->is_real()) {
			return insn_call(Type::integer, {v}, "Array.isize.1");
		} else {
			return insn_call(Type::integer, {v}, "Array.isize");
		}
	}
	// TODO : default
	return insn_call(Type::integer, {v}, "Array.isize");
}

Compiler::value Compiler::insn_get_capture(int index, const Type* type) const {
	// std::cout << "get_capture " << fun->type << " " << fun->captures.size() << " captures " << F->arg_size() << " " << type << " " << index << std::endl;
	assert(fun->parent->captures.size() > 0);
	Compiler::value closure = { F->arg_begin(), Type::any };
	return insn_call(Type::any, {closure, new_integer(index)}, "Function.get_capture");
}

Compiler::value Compiler::insn_get_capture_l(int index, const Type* type) const {
	assert(type->is_polymorphic());
	Compiler::value arg0 = {F->arg_begin(), Type::any};
	auto jit_index = new_integer(index);
	return insn_call(type->pointer(), {arg0, jit_index}, "Function.get_capture_l", true);
}

void Compiler::insn_push_array(Compiler::value array, Compiler::value value) const {
	assert(array.t->llvm(*this) == array.v->getType());
	assert(value.t->llvm(*this) == value.v->getType());
	auto element_type = array.t->element()->fold();
	if (element_type->is_bool()) {
		insn_call(Type::void_, {array, value}, "Array.vpush.0");
	} else if (element_type->is_integer()) {
		insn_call(Type::void_, {array, value}, "Array.vpush.1");
	} else if (element_type->is_real()) {
		value.t = Type::real;
		insn_call(Type::void_, {array, value}, "Array.vpush.2");
	} else {
		insn_call(Type::void_, {array, insn_convert(value, Type::any)}, "Array.vpush.3");
	}
}

Compiler::value Compiler::insn_array_at(Compiler::value array, Compiler::value index) const {
	assert(array.t->llvm(*this) == array.v->getType());
	assert(index.t->llvm(*this) == index.v->getType());
	assert(array.t->is_array());
	assert(index.t->is_number());
	auto array_type = array.v->getType()->getPointerElementType();
	auto raw_data = builder.CreateStructGEP(array_type, array.v, 5);
	auto data_base = builder.CreateLoad(raw_data);
	auto data_type = array.t->element()->llvm(*this)->getPointerTo();
	auto data = builder.CreatePointerCast(data_base, data_type);
	value r = { builder.CreateGEP(data, index.v), array.t->element()->pointer() };
	assert(r.t->llvm(*this) == r.v->getType());
	return r;
}

Compiler::value Compiler::insn_array_end(Compiler::value array) const {
	assert(array.t->llvm(*this) == array.v->getType());
	return insn_load_member(array, 6);
}

Compiler::value Compiler::insn_move_inc(Compiler::value value) const {
	assert(value.t->llvm(*this) == value.v->getType());
	if (value.t->is_mpz_ptr()) {
		return insn_clone_mpz(value);
	}
	if (value.t->must_manage_memory()) {
		if (value.t->reference) {
			insn_inc_refs(value);
			return value;
		} else {
			return insn_call(value.t, {value}, "Value.move_inc");
		}
	}
	return value;
}

Compiler::value Compiler::insn_clone_mpz(Compiler::value mpz) const {
	assert(mpz.t->llvm(*this) == mpz.v->getType());
	if (mpz.t->temporary) {
		return mpz;
	}
	auto r = new_mpz();
	insn_call(Type::void_, {r, mpz}, "Number.mpz_init_set");
	return r;
}

void Compiler::insn_delete_mpz(Compiler::value mpz) const {
	// std::cout << "delete mpz " << mpz.t << std::endl;
	assert(mpz.t->llvm(*this) == mpz.v->getType());
	insn_call(Type::void_, {mpz}, "Number.mpz_clear");
	increment_mpz_deleted();
}

Compiler::value Compiler::insn_inc_refs(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->must_manage_memory()) {
		auto previous = insn_refs(v);
		auto new_refs = insn_add(previous, new_integer(1));
		auto llvm_type = v.v->getType()->getPointerElementType();
		auto r = builder.CreateStructGEP(llvm_type, v.v, 3);
		insn_store({r, Type::integer->pointer()}, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_move(Compiler::value v) const {
	assert(check_value(v));
	if (v.t->fold()->must_manage_memory() and !v.t->temporary and !v.t->reference) {
		return insn_call(v.t->add_temporary(), {v}, "Value.move");
	}
	return v;
}
Compiler::value Compiler::insn_refs(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	assert(v.t->is_polymorphic());
	return insn_load_member(v, 3);
}
Compiler::value Compiler::insn_native(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	assert(v.t->is_polymorphic());
	return insn_load_member(v, 4);
}

Compiler::value Compiler::iterator_begin(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_array()) {
		auto it = create_entry("it", v.t->iterator());
		insn_store(it, insn_load_member(v, 5));
		return it;
	}
	else if (v.t->is_interval()) {
		auto it = create_entry("it", v.t->iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, insn_load_member(v, 5));
		return it;
	}
	else if (v.t->is_string()) {
		auto it = create_entry("it", v.t->iterator());
		insn_call(Type::void_, {v, it}, "String.iterator_begin");
		return it;
	}
	else if (v.t->is_map()) {
		auto it = create_entry("it", v.t->iterator());
		insn_store(it, insn_load_member(v, 8));
		return it;
	}
	else if (v.t->is_set()) {
		auto it = create_entry("it", v.t->iterator());
		insn_store_member(it, 0, insn_load_member(v, 8));
		insn_store_member(it, 1, new_integer(0));
		return it;
	}
	else if (v.t->is_integer()) {
		auto it = create_entry("it", Type::integer->iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, to_int(insn_pow(new_integer(10), to_int(insn_log10(v)))));
		insn_store_member(it, 2, new_integer(0));
		return it;
	}
	else if (v.t->is_long()) {
		auto it = create_entry("it", Type::long_->iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, insn_pow(new_long(10), to_int(insn_log10(v))));
		insn_store_member(it, 2, new_integer(0));
		return it;
	}
	else if (v.t->is_mpz()) {
		// jit_type_t types[3] = {VM::mpz_type, VM::mpz_type, jit_type_int};
		// auto mpz_iterator = jit_type_create_struct(types, 3, 1);
		// Compiler::value it = {jit_value_create(F, mpz_iterator), Type::mpz()_ITERATOR};
		// jit_type_free(mpz_iterator);
		// auto addr = jit_insn_address_of(F, it->v);
		// jit_insn_store_relative(F, addr, 0, v.v);
		// jit_insn_store_relative(F, addr, 16, to_long(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		// jit_insn_store_relative(F, addr, 32, new_long(0).v);
		// return it;
	}
	std::cout << "Collection type: " << v.t << std::endl;
	assert(false);
}

Compiler::value Compiler::iterator_rbegin(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	if (v.t->is_array()) {
		auto it = create_entry("it", v.t->iterator());
		auto end = insn_load_member(v, 6);
		auto previous = builder.CreateGEP(end.v, new_integer(-1).v);
		insn_store(it, {previous, Type::any});
		return it;
	}
	else if (v.t->is_interval()) {
		assert(false);
	}
	else if (v.t->is_string()) {
		assert(false);
	}
	else if (v.t->is_map()) {
		auto it = create_entry("it", v.t->iterator());
		auto end = insn_call(it.t, {v}, "Map.end");
		insn_store(it, end);
		return it;
	}
	else if (v.t->is_set()) {
		assert(false);
	}
	else if (v.t->is_integer()) {
		assert(false);
	}
	else if (v.t->is_long()) {
		assert(false);
	}
	else if (v.t->is_mpz()) {
		assert(false);
	}
	assert(false);
}

Compiler::value Compiler::iterator_end(Compiler::value v, Compiler::value it) const {
	assert(v.t->llvm(*this) == v.v->getType());
	assert(it.t->llvm(*this) == it.v->getType());
	if (v.t->is_array()) {
		return insn_pointer_eq(insn_load(it), insn_array_end(v));
	}
	else if (v.t->is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto end = insn_load_member(interval, 6);
		auto pos = insn_load_member(it, 1);
		return insn_gt(pos, end);
	}
	else if (v.t->is_string()) {
		return insn_call(Type::boolean, {it}, "String.iterator_end");
	}
	else if (v.t->is_map()) {
		auto node = insn_load(it);
		auto end = insn_call(node.t, {v}, "Map.iterator_end");
		return { builder.CreateICmpEQ(node.v, end.v), Type::boolean };
	}
	else if (v.t->is_set()) {
		auto node = insn_load_member(it, 0);
		auto end = insn_call(node.t, {v}, "Set.iterator_end");
		return {builder.CreateICmpEQ(node.v, end.v), Type::boolean};
	}
	else if (v.t->is_integer()) {
		return insn_eq(insn_load_member(it, 1), new_integer(0));
	}
	else if (v.t->is_long()) {
		return insn_eq(insn_load_member(it, 1), new_long(0));
	}
	assert(false);
}

Compiler::value Compiler::iterator_rend(Compiler::value v, Compiler::value it) const {
	assert(v.t->llvm(*this) == v.v->getType());
	assert(it.t->llvm(*this) == it.v->getType());
	if (v.t->is_array()) {
		auto before_first = builder.CreateGEP(insn_load_member(v, 5).v, new_integer(-1).v);
		return insn_pointer_eq(insn_load(it), {before_first, v.t->element()->pointer()});
	}
	else if (v.t->is_interval()) {
		assert(false);
	}
	else if (v.t->is_string()) {
		assert(false);
	}
	else if (v.t->is_map()) {
		auto node = insn_load(it);
		auto end = insn_load_member(v, 8);
		return insn_pointer_eq(node, end);
	}
	else if (v.t->is_set()) {
		assert(false);
	}
	else if (v.t->is_integer()) {
		assert(false);
	}
	else if (v.t->is_long()) {
		assert(false);
	}
	assert(false);
}

Compiler::value Compiler::iterator_get(const Type* collectionType, Compiler::value it, Compiler::value previous) const {
	assert(it.t->llvm(*this) == it.v->getType());
	assert(previous.t->llvm(*this) == previous.v->getType());
	if (collectionType->is_array()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto e = insn_load(it);
		auto f = insn_load(e);
		insn_inc_refs(f);
		return f;
	}
	if (collectionType->is_interval()) {
		return insn_load_member(it, 1);
	}
	if (collectionType->is_string()) {
		auto int_char = insn_call(Type::integer, {it}, "String.iterator_get");
		return insn_call(Type::tmp_string, {int_char, previous}, "String.iterator_get.1");
	}
	if (collectionType->is_map()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = insn_load_member(node, 5);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType->is_set()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto node = insn_load_member(it, 0);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType->is_integer()) {
		return insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1));
	}
	else if (collectionType->is_long()) {
		return to_int(insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1)));
	}
	assert(false);
}

Compiler::value Compiler::iterator_rget(const Type* collectionType, Compiler::value it, Compiler::value previous) const {
	assert(it.t->llvm(*this) == it.v->getType());
	assert(previous.t->llvm(*this) == previous.v->getType());
	if (collectionType->is_array()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto e = insn_load(it);
		auto f = insn_load(e);
		insn_inc_refs(f);
		return f;
	}
	if (collectionType->is_interval()) {
		return insn_load_member(it, 1);
	}
	if (collectionType->is_string()) {
		auto int_char = insn_call(Type::integer, {it}, "String.iterator_get");
		return insn_call(Type::string, {int_char, previous}, "String.iterator_get.1");
	}
	if (collectionType->is_map()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = [&]() { if (collectionType->element()->is_integer() and collectionType->key()->is_integer()) {
			return insn_call(collectionType->element(), {node}, "Map.iterator_rget");
		} else if (collectionType->element()->is_integer()) {
			return insn_call(collectionType->element(), {node}, "Map.iterator_rget.1");
		} else if (collectionType->element()->is_real()) {
			return insn_call(collectionType->element(), {node}, "Map.iterator_rget.2");
		} else {
			return insn_call(collectionType->element(), {node}, "Map.iterator_rget.3");
		}}();
		insn_inc_refs(e);
		return e;
	}
	if (collectionType->is_set()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto node = insn_load_member(it, 0);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType->is_integer()) {
		return insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1));
	}
	else if (collectionType->is_long()) {
		return to_int(insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1)));
	}
	assert(false);
}

Compiler::value Compiler::iterator_key(Compiler::value v, Compiler::value it, Compiler::value previous) const {
	assert(v.t->llvm(*this) == v.v->getType());
	assert(it.t->llvm(*this) == it.v->getType());
	// assert(previous.t->llvm(*this) == previous.v->getType());
	if (v.t->is_array()) {
		auto array_begin = insn_array_at(v, new_integer(0));
		if (v.t->element()->is_polymorphic()) array_begin = { builder.CreatePointerCast(array_begin.v, Type::any->pointer()->llvm(*this)), Type::any->pointer() };
		return to_int({ builder.CreatePtrDiff(insn_load(it).v, array_begin.v), Type::long_ });
	}
	if (v.t->is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto start = insn_load_member(interval, 5);
		auto e = insn_load_member(it, 1);
		return insn_sub(e, start);
	}
	if (v.t->is_string()) {
		return insn_call(Type::integer, {it}, "String.iterator_key");
	}
	if (v.t->is_map()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (v.t->is_set()) {
		return insn_load_member(it, 1);
	}
	if (v.t->is_integer()) {
		return insn_load_member(it, 2);
	}
	else if (v.t->is_long()) {
		return insn_load_member(it, 2);
	}
	assert(false);
}

Compiler::value Compiler::iterator_rkey(Compiler::value v, Compiler::value it, Compiler::value previous) const {
	assert(v.t->llvm(*this) == v.v->getType());
	assert(it.t->llvm(*this) == it.v->getType());
	// assert(previous.t->llvm(*this) == previous.v->getType());
	if (v.t->is_array()) {
		auto array_begin = insn_array_at(v, new_integer(0));
		if (v.t->element()->is_polymorphic()) array_begin = { builder.CreatePointerCast(array_begin.v, Type::any->pointer()->llvm(*this)), Type::any->pointer() };
		return to_int({ builder.CreatePtrDiff(insn_load(it).v, array_begin.v), Type::long_ });
	}
	if (v.t->is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto start = insn_load_member(interval, 5);
		auto e = insn_load_member(it, 1);
		return insn_sub(e, start);
	}
	if (v.t->is_string()) {
		return insn_call(Type::integer, {it}, "String.iterator_key");
	}
	if (v.t->is_map()) {
		if (previous.t->must_manage_memory()) {
			insn_call(Type::void_, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = insn_call(v.t->key(), {node}, "Map.iterator_rkey");
		insn_inc_refs(e);
		return e;
	}
	if (v.t->is_set()) {
		return insn_load_member(it, 1);
	}
	if (v.t->is_integer()) {
		return insn_load_member(it, 2);
	}
	else if (v.t->is_long()) {
		return insn_load_member(it, 2);
	}
	assert(false);
}

void Compiler::iterator_increment(const Type* collectionType, Compiler::value it) const {
	assert(it.t->llvm(*this) == it.v->getType());
	if (collectionType->is_array()) {
		auto it2 = insn_load(it);
		auto next_element = builder.CreateGEP(it2.v, new_integer(1).v);
		insn_store(it, {next_element, Type::any});
		return;
	}
	if (collectionType->is_interval()) {
		auto pos = insn_load_member(it, 1);
		insn_store_member(it, 1, insn_add(pos, new_integer(1)));
		return;
	}
	if (collectionType->is_string()) {
		insn_call(Type::void_, {it}, "String.iterator_next");
		return;
	}
	if (collectionType->is_map()) {
		auto node = insn_load(it);
		insn_store(it, insn_call(node.t, {node}, "Map.iterator_inc"));
		return;
	}
	if (collectionType->is_set()) {
		auto node = insn_load_member(it, 0);
		insn_store_member(it, 1, insn_add(insn_load_member(it, 1), new_integer(1)));
		insn_store_member(it, 0, insn_call(node.t, {node}, "Set.iterator_inc"));
		return;
	}
	if (collectionType->is_integer()) {
		auto n = insn_load_member(it, 0);
		auto p = insn_load_member(it, 1);
		auto i = insn_load_member(it, 2);
		insn_store_member(it, 0, insn_mod(n, p));
		insn_store_member(it, 1, insn_int_div(p, new_integer(10)));
		insn_store_member(it, 2, insn_add(i, new_integer(1)));
		return;
	}
	else if (collectionType->is_long()) {
		auto n = insn_load_member(it, 0);
		auto p = insn_load_member(it, 1);
		auto i = insn_load_member(it, 2);
		insn_store_member(it, 0, insn_mod(n, p));
		insn_store_member(it, 1, insn_int_div(p, new_long(10)));
		insn_store_member(it, 2, insn_add(i, new_integer(1)));
		return;
	}
	assert(false);
}

void Compiler::iterator_rincrement(const Type* collectionType, Compiler::value it) const {
	assert(it.t->llvm(*this) == it.v->getType());
	if (collectionType->is_array()) {
		auto it2 = insn_load(it);
		auto next_element = builder.CreateGEP(it2.v, new_integer(-1).v);
		insn_store(it, {next_element, Type::any});
		return;
	}
	if (collectionType->is_interval()) {
		assert(false);
	}
	if (collectionType->is_string()) {
		assert(false);
	}
	if (collectionType->is_map()) {
		auto node = insn_load(it);
		insn_store(it, insn_call(node.t, {node}, "Map.iterator_dec"));
		return;
	}
	if (collectionType->is_set()) {
		assert(false);
	}
	if (collectionType->is_integer()) {
		assert(false);
	}
	else if (collectionType->is_long()) {
		assert(false);
	}
	assert(false);
}

Compiler::value Compiler::insn_foreach(Compiler::value container, const Type* output, Variable* var, Variable* key, std::function<Compiler::value(Compiler::value, Compiler::value)> body, bool reversed, std::function<Compiler::value(Compiler::value, Compiler::value)> body2) {
	
	enter_block(new Block()); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	Compiler::value output_v;
	if (not output->is_void()) {
		output_v = new_array(output, {});
		insn_inc_refs(output_v);
		add_temporary_value(output_v); // Why create variable? in case of `break 2` the output must be deleted
	}
	
	insn_inc_refs(container);
	add_temporary_value(container);

	// Create variables
	auto value_var = var->val;
	if (container.t->element()->is_polymorphic()) {
		insn_store(value_var, new_null());
		add_temporary_variable(var);
	}

	Compiler::value key_var;
	if (key) {
		key_var = key->val;
		if (container.t->key()->is_polymorphic()) {
			insn_store(key_var, new_null());
			add_temporary_variable(key);
		}
	}

	auto it_label = insn_init_label("it");
	auto cond_label = insn_init_label("cond");
	auto cond2_label = insn_init_label("cond2");
	auto end_label = insn_init_label("end");
	auto loop_label = insn_init_label("loop");
	auto loop2_label = insn_init_label("loop2");

	enter_loop(&end_label, &it_label);

	auto it = reversed ? iterator_rbegin(container) : iterator_begin(container);

	// For arrays, if begin iterator is 0, jump to end directly
	if (container.t->is_array()) {
		auto empty_array = insn_pointer_eq(it, new_null_pointer(it.t));
		insn_if_new(empty_array, &end_label, &cond_label);
	} else {
		insn_branch(&cond_label);
	}

	// cond label:
	insn_label(&cond_label);
	// Condition to continue
	auto finished = reversed ? iterator_rend(container, it) : iterator_end(container, it);
	insn_if_new(finished, &end_label, &loop_label);

	// loop label:
	insn_label(&loop_label);
	// Get Value
	insn_store(value_var, reversed ? iterator_rget(container.t, it, insn_load(value_var)) : iterator_get(container.t, it, insn_load(value_var)));
	// Get Key
	if (key) {
		insn_store(key_var, reversed ? iterator_rkey(container, it, insn_load(key_var)) : iterator_key(container, it, insn_load(key_var)));
	}
	// Body
	auto body_v = body(insn_load(value_var), key ? insn_load(key_var) : value());
	if (body_v.v) {
		if (output_v.v) {
			insn_push_array(output_v, body_v);
		} else {
			insn_delete_temporary(body_v);
		}
	}
	insn_branch(&it_label);

	// it++
	insn_label(&it_label);
	reversed ? iterator_rincrement(container.t, it) : iterator_increment(container.t, it);
	// jump to cond
	insn_branch(body2 ? &cond2_label : &cond_label);
	
	// cond2 label:
	if (body2) {
		insn_label(&cond2_label);
		finished = reversed ? iterator_rend(container, it) : iterator_end(container, it);
		insn_if_new(finished, &end_label, &loop2_label);

	// loop2 label:
		insn_label(&loop2_label);
		insn_store(value_var, reversed ? iterator_rget(container.t, it, insn_load(value_var)) : iterator_get(container.t, it, insn_load(value_var)));
		if (key) insn_store(key_var, reversed ? iterator_rkey(container, it, insn_load(key_var)) : iterator_key(container, it, insn_load(key_var)));
		// Body 2
		auto body2_v = body2(insn_load(value_var), key ? insn_load(key_var) : value());
		if (body2_v.v) {
			if (output_v.v) {
				insn_push_array(output_v, body2_v);
			} else {
				insn_delete_temporary(body2_v);
			}
		}
		insn_branch(&it_label);
	}

	leave_loop();

	// end label:
	insn_label(&end_label);

	auto return_v = clone(output_v); // otherwise it is delete by the leave_block
	leave_block(); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block
	return return_v;
}

// Controls
Compiler::label Compiler::insn_init_label(std::string name) const {
	return {llvm::BasicBlock::Create(getContext(), name)};
}
void Compiler::insn_if(Compiler::value condition, std::function<void()> then, std::function<void()> elze) const {
	assert(condition.t->llvm(*this) == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_else = insn_init_label("else");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_then, elze ? &label_else : &label_end);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	if (elze) {
		insn_label(&label_else);
		elze();
		insn_branch(&label_end);
	}
	insn_label(&label_end);
	((Compiler*) this)->current_block()->blocks.push_back(label_end.block);
}
void Compiler::insn_if_new(Compiler::value cond, label* then, label* elze) const {
	assert(cond.t->llvm(*this) == cond.v->getType());
	builder.CreateCondBr(cond.v, then->block, elze->block);
}

void Compiler::insn_if_not(Compiler::value condition, std::function<void()> then) const {
	assert(condition.t->llvm(*this) == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_end, &label_then);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}

void Compiler::insn_throw(Compiler::value v) const {
	assert(v.t->llvm(*this) == v.v->getType());
	// Throw inside a try { ... } catch { ... }, jump to catch block directly
	auto catcher = find_catcher();
	if (catcher != nullptr) {
		auto fun_catchers = catchers.back();
		int deepness = 0;
		for (size_t b = fun_catchers.size(); b > 0; --b, deepness++) {
			if (fun_catchers.at(b - 1).size()) { break; }
		}
		if (deepness > 0) {
			((Compiler*) this)->delete_variables_block(deepness);
		}
		builder.CreateBr(catcher->handler);
		insert_new_generation_block();
	} else {
		delete_function_variables();
		auto line = new_long(exception_line.top());
		auto file = new_const_string(fun->parent->token->location.file->path);
		auto function_name = new_const_string(fun->parent->name);
		insn_call(Type::void_, {v, file, function_name, line}, "System.throw");
	}
}

void Compiler::insn_throw_object(vm::Exception type) const {
	insn_throw(new_integer(type));
}

void Compiler::insn_label(label* l) const {
	F->getBasicBlockList().push_back(l->block);
	builder.SetInsertPoint(l->block);
}

void Compiler::insn_branch(label* l) const {
	builder.CreateBr(l->block);
}

void Compiler::insn_return(Compiler::value v) const {
	assert(check_value(v));
	assert(v.v != nullptr);
	builder.CreateRet(v.v);
}
void Compiler::insn_return_void() const {
	builder.CreateRetVoid();
}

Compiler::value Compiler::insn_phi(const Type* type, Compiler::value v1, Compiler::label l1, Compiler::value v2, Compiler::label l2) const {
	if (!v2.v) {
		return v1;
	}
	if (!v1.v) {
		return v2;
	}
	const auto folded_type = type->fold();
	const auto llvm_type = type->llvm(*this);
	auto phi = Compiler::builder.CreatePHI(llvm_type, 2, "phi");
	if (v1.v) {
		if (v1.t->not_temporary() != folded_type->not_temporary()) {
			std::cout << "v1 type " << v1.t << ", " << folded_type << ", " << type << std::endl;
			assert(false);
		}
		phi->addIncoming(v1.v, l1.block);
	}
	if (v2.v) {
		if (v2.t->not_temporary() != folded_type->not_temporary()) {
			std::cout << "v2 type " << v2.t << ", " << folded_type << ", " << type << std::endl;
			assert(false);
		}
		phi->addIncoming(v2.v, l2.block);
	}
	return {phi, folded_type};
}

Compiler::value Compiler::insn_phi(const Type* type, Compiler::value v1, Block* b1, Compiler::value v2, Block* b2) const {
	if (!v2.v) return v1;
	if (!v1.v) return v2;
	const auto folded_type = type->fold();
	const auto llvm_type = folded_type->llvm(*this);
	auto phi = Compiler::builder.CreatePHI(llvm_type, 2, "phi");
	if (v1.v) {
		// std::cout << "v1 type " << v1.t << ", " << folded_type << ", " << type << std::endl;
		// assert(v1.t == folded_type);
		phi->addIncoming(v1.v, b1->blocks.back());
	}
	if (v2.v) {
		// std::cout << "v2 type " << v2.t << ", " << folded_type << ", " << type << std::endl;
		// assert(v2.t == folded_type);
		phi->addIncoming(v2.v, b2->blocks.back());
	}
	return {phi, folded_type};
}

// Call functions
Compiler::value Compiler::insn_invoke(const Type* return_type, std::vector<Compiler::value> args, std::string name) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(check_value(args[i]));
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t->llvm(*this));
	}
	llvm::Function* lambda;
	auto name2 = std::count(name.begin(), name.end(), '.') == 2 ? name : name + ".0";
	auto p = mappings.find({ name2, return_type });
	if (p == mappings.end()) {
		auto fun_type = llvm::FunctionType::get(return_type->llvm(*this), llvm_types, false);
		lambda = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, name2, program->module);
		((Compiler*) this)->mappings.insert({{ name2, return_type }, {(llvm::JITTargetAddress) nullptr, lambda}});
	} else {
		lambda = p->second.function;
	}
	auto continueBlock = llvm::BasicBlock::Create(getContext(), "cont", F);
	auto r = builder.CreateInvoke(lambda, continueBlock, fun->get_landing_pad(*this), llvm_args);
	builder.SetInsertPoint(continueBlock);
	if (return_type->is_void()) {
		return {};
	} else {
		value result { r, return_type };
		assert(check_value(result));
		return result;
	}
}

Compiler::value Compiler::insn_call(Compiler::value fun, std::vector<Compiler::value> args) const {
	// std::cout << "insn_call " << fun.t << " " << fun.v->getType() << std::endl;
	std::vector<llvm::Value*> llvm_args;
	if (fun.t->is_closure()) {
		args.insert(args.begin(), fun);
	}
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t->llvm(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
	}
	auto r = [&]() { if (dynamic_cast<const Function_object_type*>(fun.t)) {
		auto convert_type = Type::fun_object(fun.t->return_type(), {});
		auto fun_to_ptr = builder.CreatePointerCast(fun.v, convert_type->llvm(*this));
		auto f = builder.CreateStructGEP(convert_type->llvm(*this)->getPointerElementType(), fun_to_ptr, 5);
		auto fun_type = (llvm::FunctionType*) Type::fun(fun.t->return_type(), fun.t->arguments())->llvm(*this);
		auto function = builder.CreateLoad(f);
		auto fun_conv = builder.CreatePointerCast(function, fun_type->getPointerTo());
		return builder.CreateCall(fun_type, fun_conv, llvm_args);
	} else {
		if (fun.t->is_pointer()) {
			// std::cout << "CreateCall fun.v " << fun.v->getType() << std::endl;
			return builder.CreateCall(fun.v, llvm_args);
		} else {
			return builder.CreateCall((llvm::Function*) fun.v, llvm_args);
		}
	}}();
	// TODO tail call elimination #197
	// r->setTailCall(true);
	// r->setTailCallKind(llvm::CallInst::TailCallKind::TCK_Tail);
	if (fun.t->return_type()->is_void()) {
		return {};
	} else {
		value result { r, fun.t->return_type() };
		assert(check_value(result));
		return result;
	}
}

Compiler::value Compiler::insn_call(const Type* return_type, std::vector<Compiler::value> args, std::string name, bool readonly) const {
	std::vector<llvm::Value*> llvm_args;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(check_value(args[i]));
		llvm_args.push_back(args[i].v);
	}
	llvm::Function* lambda;
	auto name2 = std::count(name.begin(), name.end(), '.') == 2 ? name : name + ".0";
	auto p = mappings.find({ name2, return_type });
	if (p == mappings.end()) {
		std::vector<llvm::Type*> llvm_types;
		for (unsigned i = 0, e = args.size(); i != e; ++i) {
			llvm_types.push_back(args[i].t->llvm(*this));
		}
		auto fun_type = llvm::FunctionType::get(return_type->llvm(*this), llvm_types, false);
		lambda = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, name2, program->module);
		if (readonly) {
			lambda->addFnAttr(llvm::Attribute::ReadNone);
		}
		((Compiler*) this)->mappings.insert({{ name2, return_type }, {(llvm::JITTargetAddress) nullptr, lambda}});
	} else {
		lambda = p->second.function;
	}
	auto r = builder.CreateCall(lambda, llvm_args);
	if (return_type->is_void()) {
		return {};
	} else {
		value result { r, return_type };
		assert(check_value(result));
		return result;
	}
}

Compiler::value Compiler::insn_invoke(const Type* return_type, std::vector<Compiler::value> args, Compiler::value func) const {
	if (func.t->is_closure()) {
		args.insert(args.begin(), func);
	}
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t->llvm(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t->llvm(*this));
	}
	auto continueBlock = llvm::BasicBlock::Create(getContext(), "cont", F);
	auto r = [&]() { if (dynamic_cast<const Function_object_type*>(func.t)) {
		auto convert_type = (const Type*) Type::fun(return_type, {});
		auto fun_to_ptr = builder.CreatePointerCast(func.v, convert_type->llvm(*this));
		auto f = builder.CreateStructGEP(convert_type->llvm(*this)->getPointerElementType(), fun_to_ptr, 5);
		auto fun_type = llvm::FunctionType::get(return_type->llvm(*this), llvm_types, false);
		auto function = builder.CreateLoad(f);
		auto fun_conv = builder.CreatePointerCast(function, fun_type->getPointerTo());
		return builder.CreateInvoke(fun_conv, continueBlock, fun->get_landing_pad(*this), llvm_args);
	} else {
		return builder.CreateInvoke(func.v, continueBlock, fun->get_landing_pad(*this), llvm_args);
	}}();
	builder.SetInsertPoint(continueBlock);
	if (return_type->is_void()) {
		return {};
	} else {
		value result { r, return_type };
		assert(check_value(result));
		return result;
	}
}

void Compiler::function_add_capture(Compiler::value fun, Compiler::value capture) const {
	assert(fun.t->llvm(*this) == fun.v->getType());
	assert(capture.t->llvm(*this) == capture.v->getType());
	assert(fun.t->is_closure());
	assert(capture.t->is_polymorphic());
	// std::cout << "add capture " << capture.t << std::endl;
	insn_call(Type::void_, {fun, capture}, "Function.add_capture");
}

void Compiler::log(const std::string&& str) const { assert(false); }

// Blocks
void Compiler::enter_block(Block* block) {
	blocks.back().push_back(block);
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
	catchers.back().push_back({});
}
void Compiler::leave_block(bool delete_vars) {
	if (delete_vars) {
		delete_variables_block(1);
	}
	blocks.back().pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
	catchers.back().pop_back();
}
void Compiler::delete_variables_block(int deepness) {
	// std::cout << "blocks " << blocks.size() << std::endl;
	for (int i = blocks.back().size() - 1; i >= (int) blocks.back().size() - deepness; --i) {
		auto& variables = blocks.back()[i]->variables;
		for (auto it = variables.begin(); it != variables.end(); ++it) {
			// std::cout << "delete variable block " << it->second << " " << it->second->type << " " << it->second->val.v << " scope " << (int)it->second->scope << " " << it->second->assignment << std::endl;
			if (it->second->phi or it->second->assignment or not it->second->val.v or it->second->scope == VarScope::CAPTURE) continue;
			if (it->second->type->is_mpz_ptr()) {
				insn_delete_mpz(it->second->val);
			} else if (it->second->type->must_manage_memory()) {
				insn_delete(insn_load(it->second->val));
			}
		}
		for (const auto& value : blocks.back()[i]->temporary_values) {
			// std::cout << "delete temporary value " << value.t << std::endl;
			if (value.t->temporary) {
				insn_delete_temporary(value);
			} else {
				insn_delete(value);
			}
		}
		for (const auto& value : blocks.back()[i]->temporary_expression_values) {
			// std::cout << "delete temporary expression value " << value.t << std::endl;
			if (value.t->temporary) {
				insn_delete_temporary(value);
			} else {
				insn_delete(value);
			}
		}
		auto& temporary_variables = blocks.back()[i]->temporary_variables;
		for (const auto& variable : temporary_variables) {
			insn_delete_variable(variable->val);
		}
	}
}

void Compiler::enter_function(llvm::Function* F, bool is_closure, FunctionVersion* fun) {
	blocks.push_back({});
	functions.push(F);
	functions2.push(fun);
	functions_blocks.push_back(0);
	catchers.push_back({});
	function_is_closure.push(is_closure);
	auto block = builder.GetInsertBlock();
	if (!block) block = fun->block;
	function_llvm_blocks.push(block);
	exception_line.push(-1);
	this->F = F;
	this->fun = fun;
	std::vector<std::string> args;
	for (unsigned i = 0; i < fun->parent->arguments.size(); ++i) {
		args.push_back(fun->parent->arguments.at(i)->content);
	}
}

void Compiler::leave_function() {
	blocks.pop_back();
	functions.pop();
	functions2.pop();
	functions_blocks.pop_back();
	catchers.pop_back();
	function_is_closure.pop();
	exception_line.pop();
	this->F = functions.top();
	this->fun = functions2.top();
	builder.SetInsertPoint(function_llvm_blocks.top());
	function_llvm_blocks.pop();
}

int Compiler::get_current_function_blocks() const {
	return functions_blocks.back();
}
void Compiler::delete_function_variables() const {
	((Compiler*) this)->delete_variables_block(get_current_function_blocks());
}
bool Compiler::is_current_function_closure() const {
	return function_is_closure.size() ? function_is_closure.top() : false;
}
void Compiler::insert_new_generation_block() const {
	auto block = llvm::BasicBlock::Create(getContext(), "new.block", F);
	builder.SetInsertPoint(block);
}
Block* Compiler::current_block() const {
	return blocks.back().back();
}

// Variables

Compiler::value Compiler::add_external_var(Variable* variable) {
	variable->val = create_entry("ctx." + variable->name, variable->type);
	insn_store(variable->val, insn_load(get_symbol("ctx." + variable->name, variable->type->pointer())));
	return variable->val;
}

void Compiler::export_context_variable(const std::string& name, Compiler::value v) const {
	auto f = [&]() {
		if (v.t->is_integer()) return "Value.export_ctx_var.1";
		if (v.t->is_long()) return "Value.export_ctx_var.2";
		if (v.t->is_real()) return "Value.export_ctx_var.3";
		return "Value.export_ctx_var";
	}();
	insn_call(Type::void_, {new_const_string(name), v}, f);
}

void Compiler::add_temporary_variable(Variable* variable) {
	blocks.back().back()->temporary_variables.push_back(variable);
}
void Compiler::add_temporary_value(Compiler::value value) {
	blocks.back().back()->temporary_values.push_back(value);
}
void Compiler::pop_temporary_value() {
	insn_delete_temporary(blocks.back().back()->temporary_values.back());
	blocks.back().back()->temporary_values.pop_back();
}
void Compiler::add_temporary_expression_value(Compiler::value value) {
	blocks.back().back()->temporary_expression_values.push_back(value);
}
void Compiler::pop_temporary_expression_value() {
	blocks.back().back()->temporary_expression_values.pop_back();
}

// Loops
void Compiler::enter_loop(Compiler::label* end_label, Compiler::label* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
	loops_blocks.push_back(0);
}
void Compiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
	loops_blocks.pop_back();
}
Compiler::label* Compiler::get_current_loop_end_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}
Compiler::label* Compiler::get_current_loop_cond_label(int deepness) const {
	return loops_cond_labels[loops_cond_labels.size() - deepness];
}
int Compiler::get_current_loop_blocks(int deepness) const {
	int sum = 0;
	for (size_t i = loops_blocks.size() - deepness; i < loops_blocks.size(); ++i) {
		sum += loops_blocks[i];
	}
	return sum;
}

/** Operations **/
void Compiler::inc_ops(int amount) const {
	if (not vm->enable_operations) return;
	inc_ops_jit(new_integer(amount));
}
void Compiler::inc_ops_jit(Compiler::value amount) const {
	assert(amount.t->llvm(*this) == amount.v->getType());
	assert(amount.t->is_number());

	// Operations enabled?
	if (not vm->enable_operations) return;

	// Get the operations counter global variable
	auto ops_ptr = get_symbol("System.operations", Type::integer->pointer());

	// Increment counter
	auto jit_ops = insn_load(ops_ptr);

	// Compare to the limit
	auto limit = new_integer(vm->operation_limit);
	auto compare = insn_gt(jit_ops, limit);
	// If greater than the limit, throw exception
	insn_if(compare, [&]() {
		insn_throw_object(vm::Exception::OPERATION_LIMIT_EXCEEDED);
	});
	insn_store(ops_ptr, insn_add(jit_ops, amount));
}

/** Exceptions **/
void Compiler::mark_offset(int line) {
	exception_line.top() = line;
}
void Compiler::insn_try_catch(std::function<void()> try_, std::function<void()> catch_) {
	auto block = new Block();
	auto handler = llvm::BasicBlock::Create(getContext(), "catch", F);
	catchers.back().back().push_back({handler});
	enter_block(block);
	try_();
	delete_variables_block(1);
	leave_block();
	catchers.back().back().pop_back();
	auto ContBB = llvm::BasicBlock::Create(getContext(), "try.cont", F);
	builder.CreateBr(ContBB);
	builder.SetInsertPoint(handler);
	catch_();
	builder.CreateBr(ContBB);
	builder.SetInsertPoint(ContBB);
}

void Compiler::insn_check_args(std::vector<Compiler::value> args, std::vector<LSValueType> types) const {
	// TODO too much cheks sometimes
	for (size_t i = 0; i < args.size(); ++i) {
		auto arg = args[i];
		assert(check_value(arg));
		auto type = types[i];
		if (arg.t->is_polymorphic() and type != arg.t->id() and type != 0) {
			auto type = types[i];
			insn_if(insn_ne(insn_typeof(arg), new_integer(type)), [&]() {
				for (auto& a : args) {
					insn_delete_temporary(a);
				}
				insn_throw_object(vm::Exception::WRONG_ARGUMENT_TYPE);
			});
		}
	}
}

const Compiler::catcher* Compiler::find_catcher() const {
	const auto& fun_catchers = catchers.back();
	for (auto block = fun_catchers.size(); block > 0; --block) {
		if (fun_catchers.at(block - 1).size()) {
			return &fun_catchers.at(block - 1).back();
		}
	}
	return nullptr;
}

void Compiler::print_mpz(__mpz_struct value) {
	char buff[10000];
	mpz_get_str(buff, 10, &value);
	std::cout << buff;
}

bool Compiler::check_value(value v) const {
	if (v.t->is_void()) {
		return v.v == nullptr;
	} else {
		auto r = v.t->llvm(*this) == v.v->getType();
		if (!r) {
			std::cout << BOLD << "Wrong value type" << END_COLOR <<  ": " << v.t << " " << C_GREY << "ls " << END_COLOR << v.t->llvm(*this) << C_GREY << " llvm " << END_COLOR << v.v->getType() << std::endl;
		}
		return r;
	}
}

void Compiler::increment_mpz_created() const {
	// Get the mpz_created counter global variable
	auto mpz = get_symbol("mpzc", Type::integer->pointer());
	// Increment counter
	auto v = insn_load(mpz);
	insn_store(mpz, insn_add(v, new_integer(1)));
}
void Compiler::increment_mpz_deleted() const {
	// Get the mpz_deleted counter global variable
	auto mpz = get_symbol("mpzd", Type::integer->pointer());
	// Increment counter
	auto v = insn_load(mpz);
	insn_store(mpz, insn_add(v, new_integer(1)));
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const __mpz_struct v) {
		char buff[10000];
		mpz_get_str(buff, 10, &v);
		os << buff;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const std::vector<ls::Compiler::value>& values) {
		os << "[";
		for (size_t i = 0; i < values.size(); ++i) {
			if (i != 0) os << ", ";
			os << values[i].t;
		}
		os << "]";
		return os;
	}
}
