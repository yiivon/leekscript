#include "Function.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClosure.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Exception.hpp"
#include "llvm/IR/Verifier.h"
#include "../../type/RawType.hpp"
#include <string>
#include "../../vm/LSValue.hpp"

namespace ls {

int Function::id_counter = 0;

Function::Function() {
	body = nullptr;
	parent = nullptr;
	pos = 0;
	constant = true;
	function_added = false;
}

Function::~Function() {
	delete body;
	for (auto value : defaultValues) {
		delete value;
	}
	if (default_version != nullptr) {
		if (default_version->function != nullptr) {
			delete default_version->function;
		}
		delete default_version;
		default_version = nullptr;
	}
	for (const auto& version : versions) {
		delete version.second->function;
		delete version.second->body;
		delete version.second;
	}
	if (compiler != nullptr && handle_created) {
		compiler->removeModule(function_handle);
	}
}

void Function::addArgument(Token* name, Value* defaultValue) {
	arguments.push_back(std::unique_ptr<Token> { name });
	defaultValues.push_back(defaultValue);
}

Type Function::getReturnType() {
	if (current_version->type.return_type()._types.size() == 0) {
		if (!placeholder_type) {
			placeholder_type = Type::generate_new_placeholder_type();
		}
		return { placeholder_type };
	} else {
		return current_version->type.return_type();
	}
}

void Function::print_version(std::ostream& os, int indent, bool debug, const Version* version) const {
	if (version == nullptr) {
		os << "nullptr!!";
		return;
	}
	if (captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << captures[c]->name << " " << captures[c]->type();
		}
		os << "] ";
	}
	if (arguments.size() != 1) {
		os << "(";
	}
	for (unsigned i = 0; i < arguments.size(); ++i) {
		if (i > 0) os << ", ";
		os << arguments.at(i)->content;
		if (debug)
			os << " " << version->type.arguments().at(i);

		if (defaultValues.at(i) != nullptr) {
			os << " = ";
			defaultValues.at(i)->print(os);
		}
	}
	if (arguments.size() != 1) {
		os << ")";
	}
	os << " => ";
	version->body->print(os, indent, debug, true);

	if (debug) {
		os << " [" << versions.size() << " versions, " << std::boolalpha << has_version << "]";
		os << "<";
		int i = 0;
		for (const auto& v : versions) {
			if (i > 0) os << ", ";
			if (v.second == version) os << "$";
			os << v.first << " => " << v.second->type.return_type();
			i++;
		}
		os << ">";
	}
	if (debug) {
		//os << " " << type;
	}
}

void Function::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (has_version && versions.size() == 1) {
		// std::cout << "print version " << versions.begin()->second->type << std::endl;
		print_version(os, indent, debug, versions.begin()->second);
	} else {
		// std::cout << "print default version" << std::endl;
		print_version(os, indent, debug, default_version);
	}
}

Location Function::location() const {
	return body->location();
}

/*
 * When returing a function, compile a default version with all parameters
 * as pointers, when the function will be in the nature, there will be no
 * problem. Inside a block, it's possible to compile a more specific version
 * of a function in this block to speed up things

let f = x -> x + '!'
let g2 = f('hello')   // default version with pointer
let g1 = f(10)		   // version with [x = int]

let f = x -> y -> x + y
let g = f('hello ')	// default version of f
let h1 = g(10)			// here we act like we don't know where does 'g' come from, and take the default version with pointer
let h2 = g('world')	// same version, pointers

let f = function(a) {
	let g = x -> x + a
	g('hello')	// default version with pointer
	g(12)		// version with [x = int]
	return g	// here, g will go outside his parent, so we take the full-pointer version, regardless of the version of function f. So f can be recompile multiple times, it will not affect the arguments of g and its return type.
}
let r1 = f(' !')		// default version
let r2 = f(12)			// version with number, recompiler f with a [a = int] version, it will not modify the signature of g
r2(12)
r2('hello')
 */
void Function::analyse(SemanticAnalyser* analyser) {

	parent = analyser->current_function();

	if (!function_added) {
		analyser->add_function(this);
		function_added = true;
	}
	std::vector<Type> args;
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		if (defaultValues[i] != nullptr) {
			defaultValues[i]->analyse(analyser);
		}
		args.push_back(Type::ANY);
		// args.push_back(Type::generate_new_placeholder_type());
	}
	type = Type::fun({}, args, this);

	if (!default_version) {
		default_version = new Function::Version();
		current_version = default_version;
		default_version->body = body;
		if (captures.size()) {
			default_version->function = new LSClosure(nullptr);
		} else {
			default_version->function = new LSFunction(nullptr);
		}
		default_version->function->refs = 1;
		default_version->function->native = true;
	}
	analyzed = true;

	// auto return_type = req_type.getReturnType();
	analyse_body(analyser, type.arguments(), default_version);

	// Re-analyse each version
	for (auto v : versions) {
		analyse_body(analyser, v.first, v.second);
	}

	type = default_version->type;

	update_function_args(analyser);

	// std::cout << "Function type: " << type << std::endl;
}

bool Function::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {
	// std::cout << "Function " << this << " ::will_take " << args << " level " << level << std::endl;
	if (!analyzed) {
		analyse(analyser);
	}
	if (level == 1) {
		if (versions.find(args) == versions.end()) {
			for (const auto& t : args) {
				if (t.is_placeholder()) return false;
			}
			auto version = new Function::Version();
			version->body = (Block*) body->clone();
			if (captures.size()) {
				version->function = new LSClosure(nullptr);
			} else {
				version->function = new LSFunction(nullptr);
			}
			version->function->refs = 1;
			version->function->native = true;
			versions.insert({args, version});

			analyse_body(analyser, args, version);

			// std::cout << "created version type : " << version->type << std::endl;
			update_function_args(analyser);
			return true;
		}
		return false;
	} else {
		auto v = current_version ? current_version : default_version;
		if (auto ei = dynamic_cast<ExpressionInstruction*>(v->body->instructions[0])) {
			if (auto f = dynamic_cast<Function*>(ei->value)) {

				analyser->enter_function(this);
				for (unsigned i = 0; i < arguments.size(); ++i) {
					analyser->add_parameter(arguments[i].get(), v->type.argument(i));
				}
				f->will_take(analyser, args, level - 1);

				analyser->leave_function();

				if (captures.size()) {
					v->type = Type::closure(f->versions[args]->type, v->type.arguments(), this);
				} else {
					v->type = Type::fun(f->versions[args]->type, v->type.arguments(), this);
				}

				// std::cout << "Sub function type: " << f->versions.begin()->second->type << std::endl;
				// std::cout << "Main function type: " << v->type << std::endl;

				// analyse_body(analyser, args, v, Type::UNKNOWN);
			}
		}
	}
	return false;
}

void Function::set_version(const std::vector<Type>& args, int level) {
	if (level == 1) {
		version = args;
		has_version = true;
	} else {
		auto v = current_version ? current_version : default_version;
		if (auto ei = dynamic_cast<ExpressionInstruction*>(v->body->instructions[0])) {
			if (auto f = dynamic_cast<Function*>(ei->value)) {
				f->set_version(args, level - 1);
			}
		}
	}
}

void Function::analyse_body(SemanticAnalyser* analyser, std::vector<Type> args, Version* version) {

	// std::cout << "Function::analyse_body(" << args << ", " << req_type << ")" << std::endl;

	captures.clear();
	analyser->enter_function(this);
	current_version = version;

	std::vector<Type> arg_types;
	for (unsigned i = 0; i < arguments.size(); ++i) {
		Type type = i < args.size() ? args.at(i) : (i < defaultValues.size() ? defaultValues.at(i)->type : Type::ANY);
		analyser->add_parameter(arguments.at(i).get(), type);
		arg_types.push_back(type);
	}
	version->body->analyse(analyser);
	if (captures.size()) {
		version->type = Type::closure(version->body->type, arg_types, this);
	} else {
		version->type = Type::fun(version->body->type, arg_types, this);
	}
	Type return_type;
	for (const auto& t : version->body->type._types) {
		if (dynamic_cast<const Placeholder_type*>(t.get()) == nullptr) {
			return_type += t;
		}
	}
	version->body->type = return_type;
	if (captures.size()) {
		version->type = Type::closure(return_type, arg_types, this);
	} else {
		version->type = Type::fun(return_type, arg_types, this);
	}
	version->body->analyse(analyser);

	if (version->type.return_type() == Type::MPZ) {
		// version->type.setReturnType(Type::MPZ_TMP);
	}
	vars = analyser->get_local_vars();
	analyser->leave_function();

	// cout << "function analysed body : " << version->type << endl;
}

int Function::capture(std::shared_ptr<SemanticVar> var) {
	// std::cout << "Function::capture " << var->name << std::endl;

	// Function become a closure
	if (!default_version->function->closure()) {
		delete default_version->function;
		default_version->function = new LSClosure(nullptr);
		default_version->function->refs = 1;
		default_version->function->native = true;
	}
	default_version->type = Type::closure(default_version->type.return_type(), default_version->type.arguments());
	for (auto& version : versions) {
		if (!version.second->function->closure()) {
			delete version.second->function;
			version.second->function = new LSClosure(nullptr);
			version.second->function->refs = 1;
			version.second->function->native = true;
		}
		version.second->type = Type::closure(version.second->type.return_type(), version.second->type.arguments());
	}
	type = Type::closure(type.return_type(), type.arguments());

	for (size_t i = 0; i < captures.size(); ++i) {
		if (captures[i]->name == var->name)
			return i;
	}
	var = std::make_shared<SemanticVar>(*var);
	captures.push_back(var);

	if (var->function != parent) {
		auto new_var = std::make_shared<SemanticVar>(*var);
		new_var->index = parent->capture(new_var);
		var->scope = VarScope::CAPTURE;
		var->parent_index = new_var->index;
	}
	return captures.size() - 1;
}

void Function::update_function_args(SemanticAnalyser* analyser) {
	auto ls_fun = default_version->function;
	ls_fun->args.clear();
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		auto clazz = type.argument(i).clazz();
		LSClass* arg_class = (LSClass*) analyser->vm->system_vars[clazz];
		if (arg_class != nullptr) {
			ls_fun->args.push_back((LSValue*) arg_class);
		} else {
			ls_fun->args.push_back(analyser->vm->system_vars["Value"]);
		}
	}
	auto return_class_name = type.return_type().clazz();
	LSClass* return_class = (LSClass*) analyser->vm->system_vars[return_class_name];
	if (return_class != nullptr) {
		ls_fun->return_type = (LSValue*) return_class;
	} else {
		ls_fun->return_type = analyser->vm->system_vars["Value"];
	}
}

Type Function::version_type(std::vector<Type> version) const {
	if (versions.find(version) != versions.end()) {
		return versions.at(version)->type;
	}
	return type;
}

void Function::must_return(SemanticAnalyser*, const Type& type) {
	// std::cout << "Function::must_return " << type << std::endl;
	if (type == Type::ANY) {
		generate_default_version = true;
	}
	return_type = type;
}

Compiler::value Function::compile(Compiler& c) const {

	// std::cout << "Function::compile() " << this << " version " << version << " " << has_version << std::endl;
	
	((Function*) this)->compiled = true;
	((Function*) this)->compiler = &c;

	if (!is_main_function && !has_version && !generate_default_version) {
		// std::cout << "/!\\ No version! (no custom version + no default version generated)" << std::endl;
		// std::cout << "versions = " << versions.size() << std::endl;
	}

	// Compile default version
	// std::cout << "generate_default_version " << generate_default_version << std::endl;
	if (is_main_function || generate_default_version) {
		compile_version_internal(c, type.arguments(), default_version);
	}

	// Add captures (for sub functions only)
	for (auto& version : ((Function*) this)->versions) {
		compile_version_internal(c, version.first, version.second);
	}

	if (has_version) {
		return compile_version(c, version);
	} else {
		// Return default version
		return c.new_function(default_version->function, default_version->type);
	}
}

Compiler::value Function::compile_version(Compiler& c, std::vector<Type> args) const {
	// std::cout << "Function::compile_version(" << args << ")" << std::endl;
	if (!compiled) {
		compile(c);
	}
	if (versions.find(args) == versions.end()) {
		// std::cout << "/!\\ Version " << args << " not found!" << std::endl;
		return c.new_pointer(LSNull::get());
	}
	return c.new_function(versions.at(args)->function, versions.at(args)->type);
}

llvm::BasicBlock* Function::get_landing_pad(const Compiler& c) {
	// std::cout << "get_landing_pad " << current_version->type << " " << current_version->landing_pad << std::endl;
	if (current_version->landing_pad == nullptr) {
		current_version->catch_block = llvm::BasicBlock::Create(LLVMCompiler::context, "catch", c.F);
		auto savedIP = LLVMCompiler::builder.saveAndClearIP();
		current_version->landing_pad = llvm::BasicBlock::Create(LLVMCompiler::context, "lpad", c.F);
		c.builder.SetInsertPoint(current_version->landing_pad);
		auto catchAllSelector = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(LLVMCompiler::context));
		auto landingPadInst = c.builder.CreateLandingPad(llvm::StructType::get(llvm::Type::getInt8PtrTy(LLVMCompiler::context), llvm::Type::getInt32Ty(LLVMCompiler::context)), 1);
		auto LPadExn = c.builder.CreateExtractValue(landingPadInst, 0);
		current_version->exception_slot = c.CreateEntryBlockAlloca("exn.slot", llvm::Type::getInt64Ty(LLVMCompiler::context));
		c.builder.CreateStore(LPadExn, current_version->exception_slot);
		landingPadInst->addClause(catchAllSelector);
		c.builder.CreateBr(current_version->catch_block);
		c.builder.restoreIP(savedIP);
		// std::cout << "Landing pad of " << current_version->type << " created! " << current_version->landing_pad << std::endl;
	}
	return current_version->landing_pad;
}

void fake_ex_destru_fun(void*) {}
void Function::compile_version_internal(Compiler& c, std::vector<Type>, Version* version) const {
	// std::cout << "Function::compile_version_internal(" << version->type << ")" << std::endl;
	((Function*) this)->current_version = version;

	const int id = id_counter++;

	auto ls_fun = version->function;

	Compiler::value jit_fun;
	if (!is_main_function) {
		jit_fun = c.new_function(ls_fun, version->type);
		for (const auto& cap : captures) {
			Compiler::value jit_cap;
			if (cap->scope == VarScope::LOCAL) {
				auto f = dynamic_cast<Function*>(cap->value);
				if (cap->has_version && f) {
					jit_cap = f->compile_version(c, cap->version);
				} else {
					jit_cap = c.insn_load(c.get_var(cap->name));
				}
			} else if (cap->scope == VarScope::CAPTURE) {
				jit_cap = c.insn_get_capture(cap->parent_index, cap->initial_type);
			} else {
				int offset = c.is_current_function_closure() ? 1 : 0;
				jit_cap = {c.F->arg_begin() + offset + cap->index, cap->initial_type};
			}
			if (!cap->initial_type.is_polymorphic()) {
				jit_cap = c.insn_to_any({jit_cap.v, cap->initial_type});
			}
			c.function_add_capture(jit_fun, jit_cap);
		}
	}

	// System internal variables (for main function only)
	if (is_main_function) {
		for (auto var : c.vm->system_vars) {
			auto name = var.first;
			auto value = var.second;
			auto val = var.second && var.second->type == LSValue::FUNCTION ? c.new_function(value, Type::FUNCTION) : c.new_pointer(value);
			c.vm->internals.insert({name, val.v});
		}
	}

	((Function*) this)->module = std::make_shared<llvm::Module>("jit_" + name, LLVMCompiler::context);
	module->setDataLayout(((LLVMCompiler&) c).getTargetMachine().createDataLayout());
	auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(module.get());
	fpm->add(llvm::createInstructionCombiningPass());
	fpm->add(llvm::createReassociatePass());
	fpm->add(llvm::createGVNPass());
	fpm->add(llvm::createCFGSimplificationPass());
	fpm->doInitialization();

	std::vector<llvm::Type*> args;
	if (captures.size()) {
		args.push_back(Type::ANY.llvm_type()); // first arg is the function pointer
	}
	for (auto& t : version->type.arguments()) {
		args.push_back(t.llvm_type());
	}
	auto llvm_return_type = version->type.return_type().llvm_type();
	auto function_type = llvm::FunctionType::get(llvm_return_type, args, false);
	auto llvm_function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "fun_" + name + std::to_string(id), module.get());

	auto f = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(c.context), true), llvm::Function::ExternalLinkage, "__gxx_personality_v0", module.get());
	auto Int8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(c.context), c.DL.getAllocaAddrSpace());
	auto personality = llvm::ConstantExpr::getBitCast(f, Int8PtrTy);
	llvm_function->setPersonalityFn(personality);

	((Function*) this)->block = llvm::BasicBlock::Create(LLVMCompiler::context, "entry_" + name, llvm_function);
	LLVMCompiler::builder.SetInsertPoint(block);

	c.enter_function(llvm_function, captures.size() > 0, (Function*) this);

	// Create arguments
	unsigned index = 0;
	int offset = captures.size() ? -1 : 0;
	for (auto &arg : llvm_function->args()) {
		if (captures.size() && index == 0) {
			arg.setName("__fun_ptr");
		} else {
			if (offset + index < arguments.size()) {
				const auto name = arguments.at(offset + index)->content;
				const auto type = version->type.arguments().at(offset + index);
				arg.setName(name);
				auto var = c.create_entry(name, type);
				c.insn_store(var, {&arg, type});
				c.arguments.top()[name] = var;
			}
		}
		index++;
	}

	// Compile body
	auto res = version->body->compile(c);
	if (res.v) {
		c.insn_return(res);
	} else {
		c.insn_return_void();
	}

	// Catch block
	if (current_version->landing_pad != nullptr) {
		// std::cout << "Create catch block " << current_version->type << std::endl;
		c.builder.SetInsertPoint(current_version->catch_block);
		c.delete_function_variables();
		c.insn_call({}, {{c.builder.CreateLoad(current_version->exception_slot), Type::LONG}}, +[](void* ex) {
			__cxa_throw((ex + 32), (void*) &typeid(vm::ExceptionObj), &fake_ex_destru_fun);
		});
		c.builder.CreateRetVoid();
	}

	if (!is_main_function) {
		c.leave_function();
		// Create a function : 1 op
		c.inc_ops(1);
	} else {
		c.instructions_debug << "}" << std::endl;
	}

	verifyFunction(*llvm_function);
	fpm->run(*llvm_function);

	// JIT the module containing the anonymous expression, keeping a handle so we can free it later.
	((Function*) this)->function_handle = c.addModule(module);
	((Function*) this)->handle_created = true;
	// Search the JIT for the "fun" symbol.
	auto ExprSymbol = c.findSymbol("fun_" + name + std::to_string(id));
	assert(ExprSymbol && "Function not found");
	// Get the symbol's address and cast it to the right type (takes no arguments, returns a double) so we can call it as a native function.
	ls_fun->function = (void*) cantFail(ExprSymbol.getAddress());
	version->function = ls_fun;
	// std::cout << "Function '" << name << "' compiled: " << ls_fun->function << std::endl;
}

Value* Function::clone() const {
	auto f = new Function();
	f->lambda = lambda;
	f->name = name;
	f->body = (Block*) body->clone();
	for (const auto& a : arguments) {
		f->arguments.push_back(a);
	}
	for (const auto& d : defaultValues) {
		if (d != nullptr) {
			f->defaultValues.push_back(d->clone());
		} else {
			f->defaultValues.push_back(nullptr);
		}
	}
	for (const auto& v : versions) {
		auto v2 = new Version();
		if (captures.size()) {
			v2->function = new LSClosure(nullptr);
		} else {
			v2->function = new LSFunction(nullptr);
		}
		v2->function->refs = 1;
		v2->function->native = true;
		v2->body = (Block*) v.second->body->clone();
		v2->type = v.second->type;
		f->versions.insert({v.first, v2});
	}
	return f;
}

}
