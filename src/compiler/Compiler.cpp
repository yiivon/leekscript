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
#include "../../lib/utf8.h"
#include "semantic/SemanticAnalyzer.hpp"
#include "llvm/IR/GlobalVariable.h"
#include "../type/Base_type.hpp"
#include "../vm/Program.hpp"

#define log_insn(i) log_instructions && _log_insn((i))

namespace ls {

llvm::orc::ThreadSafeContext Compiler::Ctx(llvm::make_unique<llvm::LLVMContext>());
llvm::IRBuilder<> Compiler::builder(*Ctx.getContext());

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
				auto i = mappings.find(Name);
				if (i != mappings.end() && i->second.addr) {
					return llvm::JITSymbol(i->second.addr, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				}
				auto s = this->vm->resolve_symbol(Name);
				if (s) {
					return llvm::JITSymbol((llvm::JITTargetAddress) s, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				}
				if (Name == "null") return llvm::JITSymbol((llvm::JITTargetAddress) LSNull::get(), llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				if (Name == "ops") return llvm::JITSymbol((llvm::JITTargetAddress) &this->vm->operations, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
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
		return optimizeModule(std::move(M));
	}) {
		llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
	}

void Compiler::init() {
	mappings.clear();
}
void Compiler::end() {}

Compiler::value Compiler::clone(Compiler::value v) const {
	if (v.t.fold().must_manage_memory()) {
		if (v.t.reference) {
			v = insn_load(v);
		}
		auto r = insn_call(v.t.fold(), {v}, "Value.clone");
		// log_insn(4) << "clone " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	return v;
}
Compiler::value Compiler::new_null() const {
	auto null_global = program->module->getGlobalVariable("null");
	if (!null_global) {
		auto t = Type::raw_null()->llvm(*this)->getPointerElementType();
		null_global = new llvm::GlobalVariable(*program->module, t, true, llvm::GlobalValue::ExternalLinkage, nullptr, "null");
	}
	return {null_global, Type::null()};
}
Compiler::value Compiler::new_bool(bool b) const {
	return {llvm::ConstantInt::get(getContext(), llvm::APInt(1, b, false)), Type::boolean()};
}

Compiler::value Compiler::new_integer(int x) const {
	return {llvm::ConstantInt::get(getContext(), llvm::APInt(32, x, true)), Type::integer()};
}

Compiler::value Compiler::new_real(double r) const {
	return {llvm::ConstantFP::get(getContext(), llvm::APFloat(r)), Type::real()};
}

Compiler::value Compiler::new_long(long l) const {
	return {llvm::ConstantInt::get(getContext(), llvm::APInt(64, l, true)), Type::long_()};
}

Compiler::value Compiler::new_mpz() const {
	auto r = create_entry("m", Type::tmp_mpz());
	increment_mpz_created();
	insn_call({}, {r}, "Number.mpz_init");
	return r;
}

Compiler::value Compiler::new_const_string(std::string s, std::string name) const {
	return { builder.CreateGlobalStringPtr(s, name), Type::i8().pointer() };
}

Compiler::value Compiler::new_pointer(const void* p, Type type) const {
	// TODO
	// Remove this method
	assert(type.is_pointer());
	auto longp = llvm::ConstantInt::get(getContext(), llvm::APInt(64, (long) p, false));
	return { builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, longp, type.llvm_type(*this)), type };
}
Compiler::value Compiler::new_function(Type type) const {
	return insn_call(type, {new_integer(0)}, "Function.new");
}
Compiler::value Compiler::new_function(llvm::Function* f, Type type) const {
	auto fun = insn_convert({ f, type }, Type::i8().pointer());
	return insn_call(type, {fun}, "Function.new");
}
Compiler::value Compiler::new_closure(llvm::Function* f, Type type, std::vector<Compiler::value> captures) const {
	// std::cout << "new_closure " << captures << std::endl;
	Compiler::value fun = { f, type };
	auto closure = insn_call(type, {{f, Type::i8().pointer()}}, "Function.new.1");
	for (const auto& capture : captures) {
		function_add_capture(closure, capture);
	}
	return closure;
}
Compiler::value Compiler::new_class(const void* p) const {
	return new_pointer(p, Type::clazz());
}

Compiler::value Compiler::new_object() const {
	return insn_call(Type::tmp_object(), {}, "Object.new");
}

Compiler::value Compiler::new_object_class(Compiler::value clazz) const {
	return insn_call(Type::any(), {clazz}, "Object.new.1");
}

Compiler::value Compiler::new_array(Type element_type, std::vector<Compiler::value> elements) const {
	auto folded_type = element_type.fold();
	auto array_type = Type::tmp_array(element_type);
	auto array = [&]() { if (folded_type == Type::integer()) {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.0");
	} else if (folded_type == Type::real()) {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.1");
	} else {
		return insn_call(array_type, {new_integer(elements.size())}, "Array.new.2");
	}}();
	for (const auto& element : elements) {
		auto v = insn_move(insn_convert(element, folded_type));
		insn_push_array(array, v);
	}
	// size of the array + 1 operations
	inc_ops(elements.size() + 1);
	return array;
}

Compiler::value Compiler::create_entry(const std::string& name, Type type) const {
	auto t = type == Type::tmp_mpz() ? Type::tmp_mpz_ptr() : type.pointer();
	return { CreateEntryBlockAlloca(name, type.llvm_type((Compiler&) *this)), t };
}

Compiler::value Compiler::to_int(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	auto type = v.t.fold();
	if (type.is_polymorphic()) {
		return insn_invoke(Type::integer(), {v}, "Value.int");
	}
	if (type.is_integer()) {
		return v;
	}
	if (type.is_mpz_ptr()) {
		return to_int(insn_call(Type::long_(), {v}, "Number.mpz_get_si"));
	}
	if (type.is_bool()) {
		return {builder.CreateIntCast(v.v, Type::integer().llvm_type(*this), false), Type::integer()};
	}
	if (type.is_real()) {
		return {builder.CreateFPToSI(v.v, Type::integer().llvm_type(*this)), Type::integer()};
	}
	Compiler::value r {builder.CreateIntCast(v.v, Type::integer().llvm_type(*this), true), Type::integer()};
	log_insn(4) << "to_int " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::to_real(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	if (x.t.is_polymorphic()) {
		return insn_invoke(Type::real(), {x}, "Value.real");
	}
	if (x.t.is_real()) {
		return x;
	}
	if (x.t.is_bool()) {
		return {builder.CreateUIToFP(x.v, Type::real().llvm_type(*this)), Type::real()};
	}
	return {builder.CreateSIToFP(x.v, Type::real().llvm_type(*this)), Type::real()};
}

Compiler::value Compiler::to_long(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.not_temporary() == Type::long_()) {
		return v;
	}
	if (v.t.is_bool()) {
		return {builder.CreateIntCast(v.v, Type::long_().llvm_type(*this), false), Type::long_()};
	}
	if (v.t.is_integer()) {
		return {builder.CreateIntCast(v.v, Type::long_().llvm_type(*this), true), Type::long_()};
	}
	if (v.t.is_real()) {
		return {builder.CreateFPToSI(v.v, Type::long_().llvm_type(*this)), Type::long_()};
	}
	if (v.t.is_polymorphic()) {
		return insn_invoke(Type::long_(), {v}, "Value.long");
	}
	std::cout << v.t << std::endl;
	assert(false && "not converted...");
	return v;
}

Compiler::value Compiler::insn_convert(Compiler::value v, Type t) const {
	// std::cout << "convert " << v.t << " " << t.is_primitive() << " to " << t << " " << t.is_polymorphic() << std::endl;
	// assert(v.t.llvm_type(*this) == v.v->getType());
	if (!v.v) { return v; }
	if (v.t.fold().is_polymorphic() and (t.is_polymorphic() or t.is_pointer())) {
		auto rt = t;
		// TODO
		// if (v.t.temporary) rt.temporary = true;
		return { builder.CreatePointerCast(v.v, t.llvm_type(*this)), rt };
	}
	if (v.t.fold().is_primitive() && t.is_polymorphic()) {
		auto r = insn_to_any(v);
		// TODO
		// r = { builder.CreatePointerCast(r.v, t.llvm_type(*this)), t };
		return r;
	}
	if (v.t.not_temporary() == t.not_temporary()) return v;
	if (t == Type::real()) {
		return to_real(v);
	} else if (t == Type::integer()) {
		return to_int(v);
	} else if (t == Type::long_()) {
		return to_long(v);
	}
	return v;
}

// Operators wrapping
Compiler::value Compiler::insn_not(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	return {builder.CreateNot(to_int(v).v), Type::integer()};
}

Compiler::value Compiler::insn_not_bool(Compiler::value v) const {
	Compiler::value r {builder.CreateNot(insn_to_bool(v).v), Type::boolean()};
	log_insn(4) << "not_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_neg(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.is_real()) {
		return {builder.CreateFNeg(v.v), v.t};
	} else {
		return {builder.CreateNeg(v.v), v.t};
	}
}

Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::boolean()};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::boolean()};
}

Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_add(" << a.t << ", " << b.t << ")" << std::endl;
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	auto a_type = a.t.fold();
	auto b_type = b.t.fold();
	if (a_type.is_polymorphic() or b_type.is_polymorphic()) {
		return insn_invoke(Type::any(), {insn_to_any(a), insn_to_any(b)}, "Value.operator+");
	} else if (a_type.is_real() or b_type.is_real()) {
		return {builder.CreateFAdd(to_real(a).v, to_real(b).v), Type::real()};
	} else if (a_type.is_long() or b_type.is_long()) {
		return {builder.CreateAdd(to_long(a).v, to_long(b).v), Type::long_()};
	} else {
		return {builder.CreateAdd(to_int(a).v, to_int(b).v), Type::integer()};
	}
}

Compiler::value Compiler::insn_sub(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	auto a_type = a.t.fold();
	auto b_type = b.t.fold();
	if (a_type.is_polymorphic() or b_type.is_polymorphic()) {
		return insn_invoke(Type::any(), {insn_to_any(a), insn_to_any(b)}, "Value.operator-");
	} else if (a_type.is_real() or b_type.is_real()) {
		return {builder.CreateFSub(to_real(a).v, to_real(b).v, "sub"), Type::real()};
	} else {
		return {builder.CreateSub(to_int(a).v, to_int(b).v, "sub"), Type::integer()};
	}
}

Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_eq " << a.t << " == " << b.t << std::endl;
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	auto a_type = a.t.fold();
	auto b_type = b.t.fold();
	if (a_type.is_polymorphic() or b_type.is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean(), {ap, bp}, "Value.operator==");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	}
	if (a_type.is_mpz_ptr() and b_type.is_integer()) {
		auto r = insn_eq(insn_call(Type::boolean(), {a, b}, "Number._mpz_cmp_si"), new_integer(0));
		insn_delete_temporary(a);
		return r;
	} else if (a_type.is_mpz_ptr() and b_type.is_mpz_ptr()) {
		auto r = insn_eq(insn_call(Type::boolean(), {a, b}, "Number.mpz_cmp"), new_integer(0));
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		return r;
	} else if (a_type.is_real() or b_type.is_real()) {
		return {builder.CreateFCmpOEQ(to_real(a).v, to_real(b).v), Type::boolean()};
	} else if (a_type.is_long() or b_type.is_long()) {
		return {builder.CreateICmpEQ(to_long(a).v, to_long(b).v), Type::boolean()};
	} else {
		return {builder.CreateICmpEQ(to_int(a).v, to_int(b).v), Type::boolean()};
	}
}

Compiler::value Compiler::insn_pointer_eq(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_pointer());
	assert(b.t.is_pointer());
	auto p1 = builder.CreatePointerCast(a.v, llvm::Type::getInt8PtrTy(getContext()));
	auto p2 = builder.CreatePointerCast(b.v, llvm::Type::getInt8PtrTy(getContext()));
	return { builder.CreateICmpEQ(p1, p2), Type::boolean() };
}

Compiler::value Compiler::insn_ne(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	Compiler::value r {builder.CreateICmpNE(a.v, b.v), Type::boolean()};
	log_insn(4) << "ne " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_lt " << a.t << " < " << b.t << std::endl;
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	if (a.t.is_polymorphic() or b.t.is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean(), {ap, bp}, "Value.operator<");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	}
	Compiler::value r;
	if (a.t.is_mpz_ptr() and b.t.is_integer()) {
		auto r = insn_lt(insn_call(Type::integer(), {a, b}, "Number._mpz_cmp_si"), new_integer(0));
		insn_delete_temporary(a);
		return r;
	} else if (a.t.is_mpz_ptr() and b.t.is_mpz_ptr()) {
		auto res = insn_call(Type::integer(), {a, b}, "Number.mpz_cmp");
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		return insn_lt(res, new_integer(0));
	} else if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOLT(to_real(a).v, to_real(b).v), Type::boolean()};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSLT(to_long(a).v, to_long(b).v), Type::boolean()};
	} else {
		r = {builder.CreateICmpSLT(a.v, b.v), Type::boolean()};
	}
	log_insn(4) << "lt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_le(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	Compiler::value r;
	if (a.t.is_polymorphic() or b.t.is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean(), {ap, bp}, "Value.operator<=");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	} else if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOLE(to_real(a).v, to_real(b).v), Type::boolean()};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSLE(to_long(a).v, to_long(b).v), Type::boolean()};
	} else {
		r = {builder.CreateICmpSLE(a.v, b.v), Type::boolean()};
	}
	log_insn(4) << "le " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_gt(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	Compiler::value r;
	if (a.t.is_mpz_ptr() and b.t.is_integer()) {
		auto res = insn_call(Type::integer(), {a, b}, "Number._mpz_cmp_si");
		insn_delete_temporary(a);
		return insn_gt(res, new_integer(0));
	} else if (a.t.is_integer() and b.t.is_mpz_ptr()) {
		auto res = insn_call(Type::integer(), {b, a}, "Number._mpz_cmp_si");
		insn_delete_temporary(b);
		return insn_lt(res, new_integer(0));
	} else if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOGT(to_real(a).v, to_real(b).v), Type::boolean()};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSGT(to_long(a).v, to_long(b).v), Type::boolean()};
	} else {
		r = {builder.CreateICmpSGT(a.v, b.v), Type::boolean()};
	}
	log_insn(4) << "gt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_ge(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	if (a.t.is_polymorphic() or b.t.is_polymorphic()) {
		auto ap = insn_to_any(a);
		auto bp = insn_to_any(b);
		auto r = insn_call(Type::boolean(), {ap, bp}, "Value.operator>=");
		insn_delete_temporary(ap);
		insn_delete_temporary(bp);
		return r;
	}
	Compiler::value r;
	if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOGE(to_real(a).v, to_real(b).v), Type::boolean()};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSGE(to_long(a).v, to_long(b).v), Type::boolean()};
	} else {
		r = {builder.CreateICmpSGE(a.v, b.v), Type::boolean()};
	}
	log_insn(4) << "ge " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	// std::cout << "insn_mul " << a.t << " " << b.t << " " << b.v->getType() << std::endl;
	assert_value_ok(a);
	assert_value_ok(b);
	auto a_type = a.t.fold();
	auto b_type = b.t.fold();
	if (a_type.is_polymorphic() or b_type.is_polymorphic()) {
		return insn_invoke(Type::any(), {insn_to_any(a), insn_to_any(b)}, "Value.operator*");
	} else if (a_type.is_real() or b_type.is_real()) {
		return {builder.CreateFMul(to_real(a).v, to_real(b).v), Type::real()};
	} else if (a_type.is_long() or b_type.is_long()) {
		return {builder.CreateMul(to_long(a).v, to_long(b).v), Type::long_()};
	}
	return {builder.CreateMul(to_int(a).v, to_int(b).v), Type::integer()};
}

Compiler::value Compiler::insn_div(Compiler::value a, Compiler::value b) const {
	if (a.t.is_string() and b.t.is_string()) {
		return insn_call(Type::array(Type::string()), {a, b}, "Value.operator/");
	}
	auto bv = to_real(b);
	insn_if(insn_eq(bv, new_integer(0)), [&]() {
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
	});
	value r = { builder.CreateFDiv(to_real(a).v, bv.v), Type::real() };
	insn_delete_temporary(a);
	insn_delete_temporary(b);
	return r;
}

Compiler::value Compiler::insn_int_div(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateSDiv(to_long(a).v, to_long(b).v), Type::long_()};
	}
	return {builder.CreateSDiv(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_bit_and(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_bit_or(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_bit_xor(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateXor(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_shl(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateShl(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_ashr(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateAShr(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_lshr(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateLShr(a.v, b.v), Type::integer()};
}

Compiler::value Compiler::insn_mod(Compiler::value a, Compiler::value b) const {
	assert_value_ok(a);
	assert_value_ok(b);
	insn_if(insn_eq(to_real(b), new_real(0)), [&]() {
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
	});
	if (a.t.is_long() and b.t.is_long()) {
		return { builder.CreateSRem(a.v, b.v), Type::long_() };
	} else if (a.t.is_long() and b.t.is_integer()) {
		return { builder.CreateSRem(a.v, to_long(b).v), Type::long_() };
	} else if (a.t.is_integer() and b.t.is_long()) {
		return to_int({ builder.CreateSRem(to_long(a).v, to_long(b).v), Type::long_() });
	} else if (a.t.is_integer() and b.t.is_integer()) {
		return { builder.CreateSRem(a.v, b.v), Type::integer() };
	} else {
		value r = { builder.CreateFRem(to_real(a).v, to_real(b).v), Type::real() };
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		return r;
	}
}

Compiler::value Compiler::insn_double_mod(Compiler::value a, Compiler::value b) const {
	if (a.t.is_polymorphic() or b.t.is_polymorphic()) {
		return insn_invoke(Type::any(), {insn_to_any(a), insn_to_any(b)}, "Value.operator%%");
	} else {
		return insn_mod(insn_add(insn_mod(a, b), b), b);
	}
}

Compiler::value Compiler::insn_cmpl(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	return {builder.CreateSub(to_int(insn_to_bool(a)).v, to_int(insn_to_bool(b)).v), Type::integer()};
}

Compiler::value Compiler::insn_log(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::integer()) {
		return insn_call(Type::real(), {x}, "Number.m_log");
	}
	if (x.t == Type::long_()) {
		return insn_call(Type::real(), {x}, "Number.m_log.1");
	}
	return insn_call(Type::real(), {x}, "Number.m_log.2");
}

Compiler::value Compiler::insn_log10(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::real(), {x}, "Number.m_log10");
	} else if (x.t.is_long()) {
		return insn_call(Type::real(), {x}, "Number.m_log10.1");
	} else {
		return insn_call(Type::real(), {x}, "Number.m_log10.2");
	}
}

Compiler::value Compiler::insn_ceil(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::integer()) return x;
	return to_int(insn_call(Type::real(), {x}, "Number.m_ceil"));
}

Compiler::value Compiler::insn_round(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::integer()) return x;
	return to_int(insn_call(Type::real(), {x}, "Number.m_round"));
}

Compiler::value Compiler::insn_floor(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::integer()) return x;
	return to_int(insn_call(Type::real(), {x}, "Number.m_floor"));
}

Compiler::value Compiler::insn_cos(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_cos");
	}
	return insn_call(Type::real(), {x}, "Number.m_cos.2");
}

Compiler::value Compiler::insn_sin(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_sin");
	}
	return insn_call(Type::real(), {x}, "Number.m_sin.2");
}

Compiler::value Compiler::insn_tan(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_tan");
	}
	return insn_call(Type::real(), {x}, "Number.m_tan.2");
}

Compiler::value Compiler::insn_acos(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_acos");
	}
	return insn_call(Type::real(), {x}, "Number.m_acos.2");
}

Compiler::value Compiler::insn_asin(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_asin");
	}
	return insn_call(Type::real(), {x}, "Number.m_asin.2");
}

Compiler::value Compiler::insn_atan(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_atan");
	}
	return insn_call(Type::real(), {x}, "Number.m_atan.2");
}

Compiler::value Compiler::insn_pow(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type(*this) == a.v->getType());
	assert(b.t.llvm_type(*this) == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	Compiler::value r;
	if (a.t.is_real() or b.t.is_real()) {
		r = insn_call(Type::real(), {to_real(a), to_real(b)}, "Number.powdd");
	} else if (a.t.is_long()) {
		r = to_long(insn_call(Type::real(), {a, to_int(b)}, "Number.powli"));
	} else if (a.t.is_integer()) {
		r = to_int(insn_call(Type::real(), {a, b}, "Number.powii"));
	}
	log_insn(4) << "pow " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_min(Compiler::value x, Compiler::value y) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(y.t.llvm_type(*this) == y.v->getType());
	assert(x.t.is_primitive() && y.t.is_primitive());
	if (x.t == Type::integer() and y.t == Type::integer()) {
		return insn_call(Type::integer(), {x, y}, "Number.m_min");
	}
	return insn_call(Type::real(), {to_real(x), to_real(y)}, "Number.m_min.2");
}

Compiler::value Compiler::insn_max(Compiler::value x, Compiler::value y) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(y.t.llvm_type(*this) == y.v->getType());
	assert(x.t.is_primitive() && y.t.is_primitive());
	if (x.t.is_integer() and y.t.is_integer()) {
		return insn_call(Type::integer(), {x, y}, "Number.m_max");
	}
	return insn_call(Type::real(), {to_real(x), to_real(y)}, "Number.m_max.2");
}

Compiler::value Compiler::insn_exp(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_exp");
	}
	return insn_call(Type::real(), {x}, "Number.m_exp.1");
}

Compiler::value Compiler::insn_atan2(Compiler::value x, Compiler::value y) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(y.t.llvm_type(*this) == y.v->getType());
	assert(x.t.is_primitive() && y.t.is_primitive());
	if (x.t.is_integer() && y.t.is_integer()) {
		return insn_call(Type::integer(), {x, y}, "Number.m_atan2");
	}
	return insn_call(Type::real(), {to_real(x), to_real(y)}, "Number.m_atan2.2");
}

Compiler::value Compiler::insn_abs(Compiler::value x) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	if (x.t.is_polymorphic()) {
		return insn_call(Type::real(), {to_real(x)}, "Number.m_abs.2");
	} else if (x.t.is_integer()) {
		return insn_call(Type::integer(), {x}, "Number.m_abs");
	} else if (x.t.is_long()) {
		return insn_call(Type::long_(), {x}, "Number.m_abs.1");
	} else {
		return insn_call(Type::real(), {x}, "Number.m_abs.2");
	}
}

Compiler::value Compiler::insn_to_any(Compiler::value v) const {
	if (v.t.is_polymorphic()) {
		return v; // already any
	}
	if (v.t.is_long()) {
		return insn_call(Type::any(), {to_real(v)}, "Number.new");
	} else if (v.t.is_real()) {
		return insn_call(Type::any(), {v}, "Number.new");
	} else if (v.t.is_bool()) {
		return insn_call(Type::any(), {v}, "Boolean.new");
	} else if (v.t.is_mpz_ptr()) {
		if (v.t.temporary) {
			return insn_call(Type::any(), {insn_load(v)}, "Number.new.1");
		} else {
			return insn_call(Type::any(), {insn_load(v)}, "Number.new.2");
		}
	} else {
		return insn_call(Type::any(), {to_real(v)}, "Number.new");
	}
}

Compiler::value Compiler::insn_to_bool(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.is_bool()) {
		return v;
	}
	if (v.t.is_integer() or v.t.is_long()) {
		Compiler::value r {builder.CreateICmpNE(v.v, llvm::Constant::getNullValue(v.v->getType())), Type::boolean()};
		log_insn(4) << "to_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	if (v.t.is_real()) {
		Compiler::value r {builder.CreateFCmpONE(v.v, llvm::Constant::getNullValue(v.v->getType())), Type::boolean()};
		log_insn(4) << "to_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	if (v.t.is_string()) {
		return insn_call(Type::boolean(), {v}, "String.to_bool");
	}
	if (v.t.is_array()) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		return insn_call(Type::boolean(), {v}, "Array.to_bool");
	}
	if (v.t.is_function()) {
		return new_bool(true);
	}
	if (v.t.is_mpz_ptr()) {
		// TODO
		return v;
	}
	return insn_call(Type::boolean(), {v}, "Value.to_bool");
}

Compiler::value Compiler::insn_load(Compiler::value v) const {
	// std::cout << "insn_load " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(v.t.is_pointer());
	Compiler::value r { builder.CreateLoad(v.v), v.t.pointed() };
	log_insn(4) << "load " << dump_val(v) << " " << dump_val(r) << std::endl;
	assert(r.t == v.t.pointed());
	assert(r.t.llvm_type(*this) == r.v->getType());
	if (v.t.temporary) r.t.temporary = true;
	return r;
}
Compiler::value Compiler::insn_load_member(Compiler::value v, int pos) const {
	auto type = v.t.fold();
	assert(type.llvm_type(*this) == v.v->getType());
	assert(type.fold().is_pointer());
	assert(type.fold().pointed().is_struct());
	auto s = builder.CreateStructGEP(type.pointed().llvm_type(*this), v.v, pos);
	return { builder.CreateLoad(s), type.pointed().member(pos) };
}

void Compiler::insn_store(Compiler::value x, Compiler::value y) const {
	// std::cout << "insn_store " << x.t << " " << x.v->getType() << " " << y.t << std::endl;
	assert(x.t.llvm_type(*this) == x.v->getType());
	// assert(y.t.llvm_type(*this) == y.v->getType());
	// assert(x.t.pointed().fold().not_temporary() == y.t.fold().not_temporary());
	builder.CreateStore(y.v, x.v);
	log_insn(4) << "store " << dump_val(x) << " " << dump_val(y) << std::endl;
}
void Compiler::insn_store_member(Compiler::value x, int pos, Compiler::value y) const {
	assert(x.t.llvm_type(*this) == x.v->getType());
	assert(y.t.llvm_type(*this) == y.v->getType());
	assert(x.t.is_pointer());
	assert(x.t.pointed().is_struct());
	assert(x.t.pointed().member(pos) == y.t.not_temporary());
	auto s = builder.CreateStructGEP(x.t.pointed().llvm_type(*this), x.v, pos);
	builder.CreateStore(y.v, s);
}

Compiler::value Compiler::insn_typeof(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.fold().is_any()) {
		return insn_call(Type::integer(), {v}, "Value.type");
	}
	return new_integer(v.t.id());
}

Compiler::value Compiler::insn_class_of(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	auto clazz = v.t.class_name();
	if (clazz.size() and clazz != "Value") {
		return new_pointer(vm->internal_vars.at(clazz)->lsvalue, Type::clazz());
	} else {
		return insn_call(Type::clazz(), {v}, "Value.get_class");
	}
}

void Compiler::insn_delete(Compiler::value v) const {
	// std::cout << "insn_delete " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.must_manage_memory()) {
		insn_if_not(insn_native(v), [&]() {
			auto refs = insn_refs(v);
			insn_if(insn_refs(v), [&]() {
				insn_if_not(insn_dec_refs(v, refs), [&]() {
					insn_call({}, {v}, "Value.delete");
				});
			});
		});
	} else if (v.t.is_mpz_ptr()) {
		insn_delete_mpz(v);
	}
}

void Compiler::insn_delete_temporary(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t == Type::tmp_mpz_ptr()) {
		insn_delete_mpz(v);
	} if (v.t.must_manage_memory()) {
		if (v.t.temporary) {
			insn_call({}, {v}, "Value.delete");
		} else {
			insn_if_not(insn_refs(v), [&]() {
				insn_call({}, {v}, "Value.delete");
			});
		}
	}
}

Compiler::value Compiler::insn_array_size(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.is_string()) {
		return insn_call(Type::integer(), {v}, "String.isize");
	} else if (v.t.is_array()) {
		if (v.t.element().is_integer()) {
			return insn_call(Type::integer(), {v}, "Array.isize.2");
		} else if (v.t.element().is_real()) {
			return insn_call(Type::integer(), {v}, "Array.isize.1");
		} else {
			return insn_call(Type::integer(), {v}, "Array.isize");
		}
	}
	// TODO : default
	return insn_call(Type::integer(), {v}, "Array.isize");
}

Compiler::value Compiler::insn_get_capture(int index, Type type) const {
	// std::cout << "get_capture " << fun << " " << F->arg_size() << " " << type << " " << F->arg_begin()->getType() << " " << index << std::endl;
	Compiler::value arg0 = {F->arg_begin(), Type::integer()};
	auto jit_index = new_integer(index);
	auto first_type = type.is_primitive() ? Type::any() : type;
	auto v = insn_call(first_type, {arg0, jit_index}, "Function.get_capture");
	if (type.is_integer()) {
		v = insn_call(Type::integer(), {v}, "Value.get_int");
	}
	return v;
}

Compiler::value Compiler::insn_get_capture_l(int index, Type type) const {
	assert(type.is_polymorphic());
	Compiler::value arg0 = {F->arg_begin(), Type::integer()};
	auto jit_index = new_integer(index);
	return insn_call(type.pointer(), {arg0, jit_index}, "Function.get_capture_l");
}

void Compiler::insn_push_array(Compiler::value array, Compiler::value value) const {
	assert(array.t.llvm_type(*this) == array.v->getType());
	assert(value.t.llvm_type(*this) == value.v->getType());
	auto element_type = array.t.element().fold();
	if (element_type == Type::integer()) {
		insn_call({}, {array, value}, "Array.vpush.0");
	} else if (element_type == Type::real()) {
		value.t = Type::real();
		insn_call({}, {array, value}, "Array.vpush.1");
	} else {
		insn_call({}, {array, insn_convert(value, Type::any())}, "Array.vpush.2");
	}
}

Compiler::value Compiler::insn_array_at(Compiler::value array, Compiler::value index) const {
	assert(array.t.llvm_type(*this) == array.v->getType());
	assert(index.t.llvm_type(*this) == index.v->getType());
	assert(array.t.is_array());
	assert(index.t.is_number());
	auto array_type = array.v->getType()->getPointerElementType();
	auto raw_data = builder.CreateStructGEP(array_type, array.v, 5);
	auto data_base = builder.CreateLoad(raw_data);
	auto data_type = array.t.element().llvm_type(*this)->getPointerTo();
	auto data = builder.CreatePointerCast(data_base, data_type);
	value r = { builder.CreateGEP(data, index.v), array.t.element().pointer() };
	assert(r.t.llvm_type(*this) == r.v->getType());
	return r;
}

Compiler::value Compiler::insn_array_end(Compiler::value array) const {
	assert(array.t.llvm_type(*this) == array.v->getType());
	return insn_load_member(array, 6);
}

Compiler::value Compiler::insn_move_inc(Compiler::value value) const {
	assert(value.t.llvm_type(*this) == value.v->getType());
	if (value.t.is_mpz_ptr()) {
		return insn_clone_mpz(value);
	}
	if (value.t.must_manage_memory()) {
		if (value.t.reference) {
			insn_inc_refs(value);
			return value;
		} else {
			return insn_call(value.t, {value}, "Value.move_inc");
		}
	}
	if (value.t.temporary) {
		return value;
	}
	return value;
}

Compiler::value Compiler::insn_clone_mpz(Compiler::value mpz) const {
	assert(mpz.t.llvm_type(*this) == mpz.v->getType());
	if (mpz.t.temporary) {
		return mpz;
	}
	auto r = new_mpz();
	insn_call({}, {r, mpz}, "Number.mpz_init_set");
	return r;
}

void Compiler::insn_delete_mpz(Compiler::value mpz) const {
	// std::cout << "delete mpz " << mpz.t << std::endl;
	assert(mpz.t.llvm_type(*this) == mpz.v->getType());
	insn_call({}, {mpz}, "Number.mpz_clear");
	increment_mpz_deleted();
}

Compiler::value Compiler::insn_inc_refs(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	if (v.t.must_manage_memory()) {
		auto previous = insn_refs(v);
		auto new_refs = insn_add(previous, new_integer(1));
		auto llvm_type = v.v->getType()->getPointerElementType();
		auto r = builder.CreateStructGEP(llvm_type, v.v, 3);
		insn_store({r, Type::integer().pointer()}, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_dec_refs(Compiler::value v, Compiler::value previous) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(!previous.v or previous.t.llvm_type(*this) == previous.v->getType());
	if (v.t.must_manage_memory()) {
		if (previous.v == nullptr) {
			previous = insn_refs(v);
		}
		auto new_refs = insn_sub(previous, new_integer(1));
		auto llvm_type = v.v->getType()->getPointerElementType();
		auto r = builder.CreateStructGEP(llvm_type, v.v, 3);
		insn_store({r, Type::integer().pointer()}, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_move(Compiler::value v) const {
	assert_value_ok(v);
	if (v.t.fold().must_manage_memory() and !v.t.temporary and !v.t.reference) {
		// TODO avoid conversions
		return insn_convert(insn_call(Type::any(), {insn_convert(v, Type::any())}, "Value.move"), v.t.fold());
	}
	return v;
}
Compiler::value Compiler::insn_refs(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(v.t.is_polymorphic());
	return insn_load_member(v, 3);
}
Compiler::value Compiler::insn_native(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(v.t.is_polymorphic());
	return insn_load_member(v, 4);
}

Compiler::value Compiler::insn_get_argument(const std::string& name) const {
	return arguments.top().at(name);
}

Compiler::value Compiler::iterator_begin(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	log_insn_code("iterator.begin()");
	if (v.t.is_array()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store(it, insn_load_member(v, 5));
		return it;
	}
	else if (v.t.is_interval()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, insn_load_member(v, 5));
		return it;
	}
	else if (v.t.is_string()) {
		auto it = create_entry("it", v.t.iterator());
		insn_call({}, {v, it}, "String.iterator_begin");
		return it;
	}
	else if (v.t.is_map()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store(it, insn_load_member(v, 8));
		return it;
	}
	else if (v.t.is_set()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store_member(it, 0, insn_load_member(v, 8));
		insn_store_member(it, 1, new_integer(0));
		return it;
	}
	else if (v.t.is_integer()) {
		auto it = create_entry("it", Type::integer().iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, to_int(insn_pow(new_integer(10), to_int(insn_log10(v)))));
		insn_store_member(it, 2, new_integer(0));
		return it;
	}
	else if (v.t.is_long()) {
		auto it = create_entry("it", Type::long_().iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, insn_pow(new_long(10), to_int(insn_log10(v))));
		insn_store_member(it, 2, new_integer(0));
		return it;
	}
	else if (v.t.is_mpz()) {
		// jit_type_t types[3] = {VM::mpz_type, VM::mpz_type, jit_type_int};
		// auto mpz_iterator = jit_type_create_struct(types, 3, 1);
		// Compiler::value it = {jit_value_create(F, mpz_iterator), Type::mpz()_ITERATOR};
		// jit_type_free(mpz_iterator);
		// auto addr = jit_insn_address_of(F, it.v);
		// jit_insn_store_relative(F, addr, 0, v.v);
		// jit_insn_store_relative(F, addr, 16, to_long(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		// jit_insn_store_relative(F, addr, 32, new_long(0).v);
		// return it;
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_rbegin(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	log_insn_code("iterator.rbegin()");
	if (v.t.is_array()) {
		auto it = create_entry("it", v.t.iterator());
		auto end = insn_load_member(v, 6);
		auto previous = builder.CreateGEP(end.v, new_integer(-1).v);
		insn_store(it, {previous, Type::any()});
		return it;
	}
	else if (v.t.is_interval()) {
		assert(false);
	}
	else if (v.t.is_string()) {
		assert(false);
	}
	else if (v.t.is_map()) {
		auto it = create_entry("it", v.t.iterator());
		auto end = insn_call(it.t, {v}, "Map.end");
		insn_store(it, end);
		return it;
	}
	else if (v.t.is_set()) {
		assert(false);
	}
	else if (v.t.is_integer()) {
		assert(false);
	}
	else if (v.t.is_long()) {
		assert(false);
	}
	else if (v.t.is_mpz()) {
		assert(false);
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_end(Compiler::value v, Compiler::value it) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(it.t.llvm_type(*this) == it.v->getType());
	log_insn_code("iterator.end()");
	if (v.t.is_array()) {
		return insn_pointer_eq(insn_load(it), insn_array_end(v));
	}
	else if (v.t.is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto end = insn_load_member(interval, 6);
		auto pos = insn_load_member(it, 1);
		return insn_gt(pos, end);
	}
	else if (v.t.is_string()) {
		return insn_call(Type::boolean(), {it}, "String.iterator_end");
	}
	else if (v.t.is_map()) {
		auto node = insn_load(it);
		auto end = insn_call(node.t, {v}, "Map.iterator_end");
		return { builder.CreateICmpEQ(node.v, end.v), Type::boolean() };
	}
	else if (v.t.is_set()) {
		auto node = insn_load_member(it, 0);
		auto end = insn_call(node.t, {v}, "Set.iterator_end");
		return {builder.CreateICmpEQ(node.v, end.v), Type::boolean()};
	}
	else if (v.t == Type::integer()) {
		return insn_eq(insn_load_member(it, 1), new_integer(0));
	}
	else if (v.t == Type::long_()) {
		return insn_eq(insn_load_member(it, 1), new_long(0));
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_rend(Compiler::value v, Compiler::value it) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(it.t.llvm_type(*this) == it.v->getType());
	log_insn_code("iterator.rend()");
	if (v.t.is_array()) {
		auto before_first = builder.CreateGEP(insn_load_member(v, 5).v, new_integer(-1).v);
		return insn_pointer_eq(insn_load(it), {before_first, v.t.element().pointer()});
	}
	else if (v.t.is_interval()) {
		assert(false);
	}
	else if (v.t.is_string()) {
		assert(false);
	}
	else if (v.t.is_map()) {
		auto node = insn_load(it);
		auto end = insn_load_member(v, 8);
		return insn_pointer_eq(node, end);
	}
	else if (v.t.is_set()) {
		assert(false);
	}
	else if (v.t == Type::integer()) {
		assert(false);
	}
	else if (v.t == Type::long_()) {
		assert(false);
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_get(Type collectionType, Compiler::value it, Compiler::value previous) const {
	assert(it.t.llvm_type(*this) == it.v->getType());
	assert(previous.t.llvm_type(*this) == previous.v->getType());
	log_insn_code("iterator.get()");
	if (collectionType.is_array()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto e = insn_load(it);
		auto f = insn_load(e);
		insn_inc_refs(f);
		return f;
	}
	if (collectionType.is_interval()) {
		return insn_load_member(it, 1);
	}
	if (collectionType.is_string()) {
		auto int_char = insn_call(Type::integer(), {it}, "String.iterator_get");
		return insn_call(Type::tmp_string(), {int_char, previous}, "String.iterator_get.1");
	}
	if (collectionType.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = insn_load_member(node, 5);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType.is_set()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto node = insn_load_member(it, 0);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType.is_integer()) {
		return insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1));
	}
	else if (collectionType.is_long()) {
		return to_int(insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1)));
	}
	return {};
}

Compiler::value Compiler::iterator_rget(Type collectionType, Compiler::value it, Compiler::value previous) const {
	assert(it.t.llvm_type(*this) == it.v->getType());
	assert(previous.t.llvm_type(*this) == previous.v->getType());
	log_insn_code("iterator.get()");
	if (collectionType.is_array()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto e = insn_load(it);
		auto f = insn_load(e);
		insn_inc_refs(f);
		return f;
	}
	if (collectionType.is_interval()) {
		return insn_load_member(it, 1);
	}
	if (collectionType.is_string()) {
		auto int_char = insn_call(Type::integer(), {it}, "String.iterator_get");
		return insn_call(Type::string(), {int_char, previous}, "String.iterator_get.1");
	}
	if (collectionType.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = [&]() { if (collectionType.element().is_integer() and collectionType.key().is_integer()) {
			return insn_call(collectionType.element(), {node}, "Map.iterator_rget");
		} else if (collectionType.element().is_integer()) {
			return insn_call(collectionType.element(), {node}, "Map.iterator_rget.1");
		} else if (collectionType.element().is_real()) {
			return insn_call(collectionType.element(), {node}, "Map.iterator_rget.2");
		} else {
			return insn_call(collectionType.element(), {node}, "Map.iterator_rget.3");
		}}();
		insn_inc_refs(e);
		return e;
	}
	if (collectionType.is_set()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto node = insn_load_member(it, 0);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType.is_integer()) {
		return insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1));
	}
	else if (collectionType.is_long()) {
		return to_int(insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1)));
	}
	return {};
}

Compiler::value Compiler::iterator_key(Compiler::value v, Compiler::value it, Compiler::value previous) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(it.t.llvm_type(*this) == it.v->getType());
	// assert(previous.t.llvm_type(*this) == previous.v->getType());
	log_insn_code("iterator.key()");
	if (v.t.is_array()) {
		auto array_begin = insn_array_at(v, new_integer(0));
		if (v.t.element().is_polymorphic()) array_begin = { builder.CreatePointerCast(array_begin.v, Type::any().pointer().llvm_type(*this)), Type::any().pointer() };
		return to_int({ builder.CreatePtrDiff(insn_load(it).v, array_begin.v), Type::long_() });
	}
	if (v.t.is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto start = insn_load_member(interval, 5);
		auto e = insn_load_member(it, 1);
		return insn_sub(e, start);
	}
	if (v.t.is_string()) {
		return insn_call(Type::integer(), {it}, "String.iterator_key");
	}
	if (v.t.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (v.t.is_set()) {
		return insn_load_member(it, 1);
	}
	if (v.t.is_integer()) {
		return insn_load_member(it, 2);
	}
	else if (v.t.is_long()) {
		return insn_load_member(it, 2);
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_rkey(Compiler::value v, Compiler::value it, Compiler::value previous) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
	assert(it.t.llvm_type(*this) == it.v->getType());
	// assert(previous.t.llvm_type(*this) == previous.v->getType());
	log_insn_code("iterator.key()");
	if (v.t.is_array()) {
		auto array_begin = insn_array_at(v, new_integer(0));
		if (v.t.element().is_polymorphic()) array_begin = { builder.CreatePointerCast(array_begin.v, Type::any().pointer().llvm_type(*this)), Type::any().pointer() };
		return to_int({ builder.CreatePtrDiff(insn_load(it).v, array_begin.v), Type::long_() });
	}
	if (v.t.is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto start = insn_load_member(interval, 5);
		auto e = insn_load_member(it, 1);
		return insn_sub(e, start);
	}
	if (v.t.is_string()) {
		return insn_call(Type::integer(), {it}, "String.iterator_key");
	}
	if (v.t.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, "Value.delete_previous");
		}
		auto node = insn_load(it);
		auto e = insn_call(v.t.key(), {node}, "Map.iterator_rkey");
		insn_inc_refs(e);
		return e;
	}
	if (v.t.is_set()) {
		return insn_load_member(it, 1);
	}
	if (v.t.is_integer()) {
		return insn_load_member(it, 2);
	}
	else if (v.t.is_long()) {
		return insn_load_member(it, 2);
	}
	return {nullptr, {}};
}

void Compiler::iterator_increment(Type collectionType, Compiler::value it) const {
	assert(it.t.llvm_type(*this) == it.v->getType());
	log_insn_code("iterator.increment()");
	if (collectionType.is_array()) {
		auto it2 = insn_load(it);
		auto next_element = builder.CreateGEP(it2.v, new_integer(1).v);
		insn_store(it, {next_element, Type::any()});
		return;
	}
	if (collectionType.is_interval()) {
		auto pos = insn_load_member(it, 1);
		insn_store_member(it, 1, insn_add(pos, new_integer(1)));
		return;
	}
	if (collectionType.is_string()) {
		insn_call({}, {it}, "String.iterator_next");
		return;
	}
	if (collectionType.is_map()) {
		auto node = insn_load(it);
		insn_store(it, insn_call(node.t, {node}, "Map.iterator_inc"));
		return;
	}
	if (collectionType.is_set()) {
		auto node = insn_load_member(it, 0);
		insn_store_member(it, 1, insn_add(insn_load_member(it, 1), new_integer(1)));
		insn_store_member(it, 0, insn_call(node.t, {node}, "Set.iterator_inc"));
		return;
	}
	if (collectionType.is_integer()) {
		auto n = insn_load_member(it, 0);
		auto p = insn_load_member(it, 1);
		auto i = insn_load_member(it, 2);
		insn_store_member(it, 0, insn_mod(n, p));
		insn_store_member(it, 1, insn_int_div(p, new_integer(10)));
		insn_store_member(it, 2, insn_add(i, new_integer(1)));
	}
	else if (collectionType.is_long()) {
		auto n = insn_load_member(it, 0);
		auto p = insn_load_member(it, 1);
		auto i = insn_load_member(it, 2);
		insn_store_member(it, 0, insn_mod(n, p));
		insn_store_member(it, 1, insn_int_div(p, new_long(10)));
		insn_store_member(it, 2, insn_add(i, new_integer(1)));
	}
}

void Compiler::iterator_rincrement(Type collectionType, Compiler::value it) const {
	assert(it.t.llvm_type(*this) == it.v->getType());
	log_insn_code("iterator.rincrement()");
	if (collectionType.is_array()) {
		auto it2 = insn_load(it);
		auto next_element = builder.CreateGEP(it2.v, new_integer(-1).v);
		insn_store(it, {next_element, Type::any()});
		return;
	}
	if (collectionType.is_interval()) {
		assert(false);
	}
	if (collectionType.is_string()) {
		assert(false);
	}
	if (collectionType.is_map()) {
		auto node = insn_load(it);
		insn_store(it, insn_call(node.t, {node}, "Map.iterator_dec"));
		return;
	}
	if (collectionType.is_set()) {
		assert(false);
	}
	if (collectionType.is_integer()) {
		assert(false);
	}
	else if (collectionType.is_long()) {
		assert(false);
	}
}

Compiler::value Compiler::insn_foreach(Compiler::value container, Type output, const std::string var, const std::string key, std::function<Compiler::value(Compiler::value, Compiler::value)> body, bool reversed) {
	
	enter_block(); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	Compiler::value output_v;
	if (not output.is_void()) {
		output_v = new_array(output, {});
		insn_inc_refs(output_v);
		add_var("{output}", output_v); // Why create variable? in case of `break 2` the output must be deleted
	}
	
	insn_inc_refs(container);
	add_var("{array}", container);

	// Create variables
	auto value_var = create_and_add_var(var, container.t.element());
	if (container.t.element().is_polymorphic()) {
		insn_store(value_var, new_null());
	}

	Compiler::value key_var;
	if (key.size()) {
		key_var = create_and_add_var(key, container.t.key());
		if (container.t.key().is_polymorphic()) {
			insn_store(key_var, new_null());
		}
	}

	auto it_label = insn_init_label("it");
	auto cond_label = insn_init_label("cond");
	auto end_label = insn_init_label("end");
	auto loop_label = insn_init_label("loop");

	enter_loop(&end_label, &it_label);

	auto it = reversed ? iterator_rbegin(container) : iterator_begin(container);

	// For arrays, if begin iterator is 0, jump to end directly
	if (container.t.is_array()) {
		auto empty_array = insn_pointer_eq(it, new_pointer(nullptr, Type::any()));
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
	if (key.size()) {
		insn_store(key_var, reversed ? iterator_rkey(container, it, insn_load(key_var)) : iterator_key(container, it, insn_load(key_var)));
	}
	// Body
	auto body_v = body(insn_load(value_var), key.size() ? insn_load(key_var) : value());
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
	insn_branch(&cond_label);

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
	assert(condition.t.llvm_type(*this) == condition.v->getType());
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
}
void Compiler::insn_if_new(Compiler::value cond, label* then, label* elze) const {
	assert(cond.t.llvm_type(*this) == cond.v->getType());
	builder.CreateCondBr(cond.v, then->block, elze->block);
}

void Compiler::insn_if_not(Compiler::value condition, std::function<void()> then) const {
	assert(condition.t.llvm_type(*this) == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_end, &label_then);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}

void Compiler::insn_throw(Compiler::value v) const {
	assert(v.t.llvm_type(*this) == v.v->getType());
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
		auto line = new_long(exception_line);
		auto function_name = new_const_string(fun->name, "fun");
		insn_call({}, {v, function_name, line}, "System.throw");
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
	assert_value_ok(v);
	assert(v.v != nullptr);
	builder.CreateRet(v.v);
}
void Compiler::insn_return_void() const {
	builder.CreateRetVoid();
}

Compiler::value Compiler::insn_phi(Type type, Compiler::value v1, Compiler::label l1, Compiler::value v2, Compiler::label l2) const {
	if (!v2.v) {
		return v1;
	}
	if (!v1.v) {
		return v2;
	}
	const auto folded_type = type.fold();
	const auto llvm_type = type.llvm_type(*this);
	auto phi = Compiler::builder.CreatePHI(llvm_type, 2, "phi");
	if (v1.v) {
		assert(v1.t == folded_type);
		phi->addIncoming(v1.v, l1.block);
	}
	if (v2.v) {
		assert(v2.t == folded_type);
		phi->addIncoming(v2.v, l2.block);
	}
	return {phi, folded_type};
}

// Call functions
Compiler::value Compiler::insn_invoke(Type return_type, std::vector<Compiler::value> args, std::string name) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert_value_ok(args[i]);
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type(*this));
	}
	llvm::Function* lambda;
	auto p = mappings.find(name);
	if (p == mappings.end()) {
		auto fun_type = llvm::FunctionType::get(return_type.llvm_type(*this), llvm_types, false);
		lambda = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, name, program->module);
		((Compiler*) this)->mappings.insert({name, {(llvm::JITTargetAddress) nullptr, lambda}});
	} else {
		lambda = p->second.function;
	}
	auto continueBlock = llvm::BasicBlock::Create(getContext(), "cont", F);
	auto r = builder.CreateInvoke(lambda, continueBlock, fun->get_landing_pad(*this), llvm_args);
	builder.SetInsertPoint(continueBlock);
	if (return_type.is_void()) {
		return {};
	} else {
		value result = { r, return_type };
		if (return_type.llvm_type(*this) != lambda->getReturnType()) {
			result.v = builder.CreatePointerCast(r, return_type.llvm_type(*this));
		}
		assert_value_ok(result);
		return result;
	}
}

Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, Compiler::value func) const {
	if (func.t.is_closure()) {
		args.insert(args.begin(), func);
	}
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t.llvm_type(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type(*this));
	}
	auto convert_type = Type::fun(return_type, {});
	auto fun_to_ptr = builder.CreatePointerCast(func.v, convert_type.llvm_type(*this));
	auto f = builder.CreateStructGEP(convert_type.llvm_type(*this)->getPointerElementType(), fun_to_ptr, 5);
	auto fun_type = llvm::FunctionType::get(return_type.llvm_type(*this), llvm_types, false);
	value function = { builder.CreateLoad(f), convert_type };
	auto fun_conv = builder.CreatePointerCast(function.v, fun_type->getPointerTo());
	auto r = builder.CreateCall(fun_type, fun_conv, llvm_args);
	if (return_type._types.size() == 0) {
		return {};
	} else {
		value result = { r, return_type };
		assert(result.t.llvm_type(*this) == result.v->getType());
		return result;
	}
}

Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, llvm::Function* fun) const {
	// std::cout << "insn_call " << fun->getName().str() << std::endl;
	std::vector<llvm::Value*> llvm_args;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t.llvm_type(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
	}
	auto r = builder.CreateCall(fun, llvm_args);
	if (return_type.is_void()) {
		return {};
	} else {
		value result = { r, return_type };
		if (return_type.llvm_type(*this) != fun->getReturnType()) {
			result.v = builder.CreatePointerCast(r, return_type.llvm_type(*this));
		}
		assert(result.t.llvm_type(*this) == result.v->getType());
		return result;
	}
}

Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, std::string name) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		// assert(args[i].t.llvm_type(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type(*this));
	}
	llvm::Function* lambda;
	auto p = mappings.find(name);
	if (p == mappings.end()) {
		auto fun_type = llvm::FunctionType::get(return_type.llvm_type(*this), llvm_types, false);
		lambda = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, name, program->module);
		((Compiler*) this)->mappings.insert({name, {(llvm::JITTargetAddress) nullptr, lambda}});
	} else {
		lambda = p->second.function;
	}
	auto r = builder.CreateCall(lambda, llvm_args);
	if (return_type.is_void()) {
		return {};
	} else {
		value result = { r, return_type };
		if (return_type.llvm_type(*this) != lambda->getReturnType()) {
			result.v = builder.CreatePointerCast(r, return_type.llvm_type(*this));
		}
		assert(result.t.llvm_type(*this) == result.v->getType());
		return result;
	}
}

Compiler::value Compiler::insn_invoke(Type return_type, std::vector<Compiler::value> args, Compiler::value func) const {
	if (func.t.is_closure()) {
		args.insert(args.begin(), func);
	}
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t.llvm_type(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type(*this));
	}
	auto convert_type = Type::fun(return_type, {});
	auto fun_to_ptr = builder.CreatePointerCast(func.v, convert_type.llvm_type(*this));
	auto f = builder.CreateStructGEP(convert_type.llvm_type(*this)->getPointerElementType(), fun_to_ptr, 5);
	auto fun_type = llvm::FunctionType::get(return_type.llvm_type(*this), llvm_types, false);
	value function = { builder.CreateLoad(f), convert_type };
	auto fun_conv = builder.CreatePointerCast(function.v, fun_type->getPointerTo());
	auto continueBlock = llvm::BasicBlock::Create(getContext(), "cont", F);
	auto r = builder.CreateInvoke(fun_conv, continueBlock, fun->get_landing_pad(*this), llvm_args);
	builder.SetInsertPoint(continueBlock);
	if (return_type._types.size() == 0) {
		return {};
	} else {
		value result = { r, return_type };
		assert_value_ok(result);
		return result;
	}
}

Compiler::value Compiler::insn_invoke(Type return_type, std::vector<Compiler::value> args, llvm::Function* function) const {
	// std::cout << "insn_invoke " << args << std::endl;
	std::vector<llvm::Value*> llvm_args;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		// assert(args[i].t.llvm_type(*this) == args[i].v->getType());
		llvm_args.push_back(args[i].v);
	}
	auto continueBlock = llvm::BasicBlock::Create(getContext(), "cont", F);
	auto r = builder.CreateInvoke(function, continueBlock, fun->get_landing_pad(*this), llvm_args);
	builder.SetInsertPoint(continueBlock);
	if (return_type.is_void()) {
		return {};
	} else {
		value result = { r, return_type };
		if (return_type.llvm_type(*this) != function->getReturnType()) {
			result.v = builder.CreatePointerCast(r, return_type.llvm_type(*this));
		}
		assert_value_ok(result);
		return result;
	}
}

void Compiler::function_add_capture(Compiler::value fun, Compiler::value capture) const {
	assert(fun.t.llvm_type(*this) == fun.v->getType());
	assert(capture.t.llvm_type(*this) == capture.v->getType());
	// std::cout << "add capture " << capture.t << std::endl;
	insn_call({}, {fun, capture}, "Function.add_capture");
}

void Compiler::log(const std::string&& str) const { assert(false); }

// Blocks
void Compiler::enter_block() {
	variables.push_back(std::map<std::string, value> {});
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
	catchers.back().push_back({});
}
void Compiler::leave_block(bool delete_vars) {
	if (delete_vars) {
		delete_variables_block(1);
		// for (const auto& var : variables.back()) {
		// 	remove_function_var(var.second);
		// }
	}
	variables.pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
	catchers.back().pop_back();
}
void Compiler::delete_variables_block(int deepness) {
	for (int i = variables.size() - 1; i >= (int) variables.size() - deepness; --i) {
		for (auto it = variables[i].begin(); it != variables[i].end(); ++it) {
			// std::cout << "delete variable block " << it->first << " " << it->second.t << " " << it->second.v->getType() << std::endl;
			if (it->second.t.is_mpz_ptr()) {
				insn_delete_mpz(it->second);
			} else if (it->second.t.pointed().must_manage_memory()) {
				insn_delete(insn_load(it->second));
			} 
		}
	}
}

void Compiler::enter_function(llvm::Function* F, bool is_closure, Function* fun) {
	variables.push_back(std::map<std::string, value> {});
	function_variables.push_back(std::vector<value> {});
	functions.push(F);
	functions2.push(fun);
	functions_blocks.push_back(0);
	catchers.push_back({});
	function_is_closure.push(is_closure);
	auto block = builder.GetInsertBlock();
	if (!block) block = fun->current_version->block;
	function_llvm_blocks.push(block);
	this->F = F;
	this->fun = fun;
	std::vector<std::string> args;
	log_insn(0) << "function " << fun->name << "(";
	for (unsigned i = 0; i < fun->arguments.size(); ++i) {
		log_insn(0) << fun->arguments.at(i)->content;
		if (i < fun->arguments.size() - 1) log_insn(0) << ", ";
		args.push_back(fun->arguments.at(i)->content);
	}
	arguments.push({});
	log_insn(0) << ") {" << std::endl;
}

void Compiler::leave_function() {
	variables.pop_back();
	function_variables.pop_back();
	functions.pop();
	functions2.pop();
	functions_blocks.pop_back();
	catchers.pop_back();
	function_is_closure.pop();
	arguments.pop();
	this->F = functions.top();
	this->fun = functions2.top();
	builder.SetInsertPoint(function_llvm_blocks.top());
	function_llvm_blocks.pop();
	log_insn(0) << "}" << std::endl;
}

int Compiler::get_current_function_blocks() const {
	return functions_blocks.back();
}
void Compiler::delete_function_variables() const {
	for (const auto& v : function_variables.back()) {
		// std::cout << "delete function variable " << v.t << std::endl;
		insn_delete(insn_load(v));
	}
}
bool Compiler::is_current_function_closure() const {
	return function_is_closure.size() ? function_is_closure.top() : false;
}
void Compiler::insert_new_generation_block() const {
	auto block = llvm::BasicBlock::Create(getContext(), "new.block", F);
	builder.SetInsertPoint(block);
}

// Variables
Compiler::value Compiler::add_var(const std::string& name, Compiler::value value) {
	assert((value.v != nullptr) && "value must not be null");
	assert(value.t.llvm_type(*this) == value.v->getType());
	auto var = create_entry(name, value.t);
	insn_store(var, value);
	variables.back()[name] = var;
	return var;
}

Compiler::value Compiler::create_and_add_var(const std::string& name, Type type) {
	// std::cout << "Compiler::create_and_add_var(" << name << ", " << type << ")" << std::endl;
	auto var = create_entry(name, type.not_temporary());
	variables.back()[name] = var;
	return var;
}

void Compiler::add_function_var(Compiler::value value) {
	assert(value.t.llvm_type(*this) == value.v->getType());
	function_variables.back().push_back(value);
}

void Compiler::remove_function_var(Compiler::value value) {
	assert(value.t.llvm_type(*this) == value.v->getType());
	auto& vars = function_variables.back();
	auto position = std::find(vars.begin(), vars.end(), value);
	if (position != vars.end())
		vars.erase(position);
}

Compiler::value Compiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			return it->second;
		}
	}
	std::cout << "var '" << name << "' not found !" << std::endl;
	assert(false && "var not found!");
}

void Compiler::update_var(std::string& name, Compiler::value v) {
	assert(v.t.llvm_type(*this) == v.v->getType());
	for (int i = variables.size() - 1; i >= 0; --i) {
		if (variables[i].find(name) != variables[i].end()) {
			auto& var = variables[i].at(name);
			bool is_mpz = v.t.is_mpz_ptr();
			if (var.t != v.t.pointer()) {
				var.t = is_mpz ? v.t : v.t.pointer();
				auto var_type = is_mpz ? Type::mpz() : v.t;
				var.v = CreateEntryBlockAlloca(name, var_type.llvm_type(*this));
			}
			if (is_mpz) {
				v = insn_load(v);
			}
			insn_store(var, v);
		}
	}
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
	assert(amount.t.llvm_type(*this) == amount.v->getType());
	assert(amount.t.is_number());

	// Operations enabled?
	if (not vm->enable_operations) return;

	// Get the operations counter global variable
	Compiler::value ops_ptr = { program->module->getGlobalVariable("ops"), Type::integer().pointer() };
	if (!ops_ptr.v) {
		auto t = Type::integer().llvm_type(*this);
		ops_ptr.v = new llvm::GlobalVariable(*program->module, t, false, llvm::GlobalValue::ExternalLinkage, nullptr, "ops");
	}

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
	exception_line = line;
}
void Compiler::insn_try_catch(std::function<void()> try_, std::function<void()> catch_) {
	auto handler = llvm::BasicBlock::Create(getContext(), "catch", F);
	catchers.back().back().push_back({handler});
	try_();
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
		assert(arg.t.llvm_type(*this) == arg.v->getType());
		auto type = types[i];
		if (arg.t.is_polymorphic() and type != arg.t.id() and type != 0) {
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

// Utils
std::ostringstream& Compiler::_log_insn(int indent) const { assert(false); }

std::string Compiler::dump_val(Compiler::value v) const { assert(false); }

void Compiler::register_label(label* v) const { assert(false); }

void Compiler::log_insn_code(std::string instruction) const {
	log_insn(0) << C_BLUE << instruction << END_COLOR << std::endl;
}
void Compiler::add_literal(void* ptr, std::string value) const {
	((Compiler*) this)->literals.insert({ptr, value});
}

void Compiler::print_mpz(__mpz_struct value) {
	char buff[10000];
	mpz_get_str(buff, 10, &value);
	std::cout << buff;
}

void Compiler::assert_value_ok(value v) const {
	if (v.t.is_void()) {
		assert(v.v == nullptr);
	} else {
		assert(v.t.llvm_type(*this) == v.v->getType());
	}
}

void Compiler::increment_mpz_created() const {
	// Get the mpz_created counter global variable
	Compiler::value mpz = { program->module->getGlobalVariable("mpzc"), Type::integer().pointer() };
	if (!mpz.v) {
		auto t = Type::integer().llvm_type(*this);
		mpz.v = new llvm::GlobalVariable(*program->module, t, false, llvm::GlobalValue::ExternalLinkage, nullptr, "mpzc");
	}
	// Increment counter
	auto v = insn_load(mpz);
	insn_store(mpz, insn_add(v, new_integer(1)));
}
void Compiler::increment_mpz_deleted() const {
	// Get the mpz_deleted counter global variable
	Compiler::value mpz = { program->module->getGlobalVariable("mpzd"), Type::integer().pointer() };
	if (!mpz.v) {
		auto t = Type::integer().llvm_type(*this);
		mpz.v = new llvm::GlobalVariable(*program->module, t, false, llvm::GlobalValue::ExternalLinkage, nullptr, "mpzd");
	}
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
