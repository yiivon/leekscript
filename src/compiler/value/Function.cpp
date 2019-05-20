#include "Function.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClosure.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Exception.hpp"
#include "llvm/IR/Verifier.h"
#include <string>
#include <fstream>
#include "../../vm/LSValue.hpp"
#include "../../type/Placeholder_type.hpp"
#include "../semantic/Callable.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../../colors.h"

namespace ls {

bool Function::Version::is_compiled() const {
	return f != nullptr;
}

int Function::id_counter = 0;

Function::Function() {
	body = nullptr;
	parent = nullptr;
	constant = true;
	function_added = false;
}

Function::~Function() {
	delete body;
	for (auto value : defaultValues) {
		delete value;
	}
	if (default_version != nullptr) {
		delete default_version;
		default_version = nullptr;
	}
	for (const auto& version : versions) {
		delete version.second->body;
		delete version.second;
	}
}

void Function::addArgument(Token* name, Value* defaultValue) {
	arguments.push_back(std::unique_ptr<Token> { name });
	defaultValues.push_back(defaultValue);
	if (defaultValue) default_values_count++;
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

void Function::print_version(std::ostream& os, int indent, bool debug, bool condensed, const Version* version) const {
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
	if (version->body->throws) {
		os << BLUE_BOLD << " throws" << END_COLOR;
	}
	os << " => ";
	version->body->print(os, indent, debug, condensed);

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
		print_version(os, indent, debug, condensed, versions.begin()->second);
	} else {
		// std::cout << "print default version" << std::endl;
		print_version(os, indent, debug, condensed, default_version);
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
void Function::analyze(SemanticAnalyzer* analyzer) {

	parent = analyzer->current_function();

	if (!function_added) {
		analyzer->add_function(this);
		function_added = true;
	}
	std::vector<Type> args;
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		if (defaultValues[i] != nullptr) {
			defaultValues[i]->analyze(analyzer);
		}
		if (!placeholder_type) {
			placeholder_type = Type::generate_new_placeholder_type();
		}
		args.push_back(placeholder_type);
	}
	type = Type::fun({}, args, this);

	if (!default_version) {
		default_version = new Function::Version();
		default_version->parent = this;
		current_version = default_version;
		default_version->body = body;
		if (captures.size()) {
			default_version->type = Type::closure(getReturnType(), args, this);
		} else {
			default_version->type = Type::fun(getReturnType(), args, this);
		}
	}
	analyzed = true;

	analyze_body(analyzer, type.arguments(), default_version);

	// Re-analyze each version
	for (auto v : versions) {
		analyze_body(analyzer, v.first, v.second);
	}

	type = default_version->type;

	// std::cout << "Function type: " << type << std::endl;
}

void Function::create_version(SemanticAnalyzer* analyzer, std::vector<Type> args) {
	// std::cout << "Function::create_version(" << args << ")" << std::endl;
	// TODO should be ==
	// assert(args.size() >= arguments.size());
	auto version = new Function::Version();
	version->parent = this;
	version->body = (Block*) body->clone();
	versions.insert({args, version});

	analyze_body(analyzer, args, version);
}

bool Function::will_take(SemanticAnalyzer* analyzer, const std::vector<Type>& args, int level) {
	// std::cout << "Function " << " ::will_take " << args << " level " << level << std::endl;
	if (!analyzed) {
		analyze(analyzer);
	}
	if (level == 1) {
		auto version = args;
		// Fill with default arguments
		for (size_t i = version.size(); i < arguments.size(); ++i) {
			if (defaultValues.at(i)) {
				version.push_back(defaultValues.at(i)->type);
			}
		}
		if (versions.find(version) == versions.end()) {
			for (const auto& t : version) {
				if (t.is_placeholder()) return false;
			}
			create_version(analyzer, version);
			return true;
		}
		return false;
	} else {
		auto v = current_version ? current_version : default_version;
		if (auto ei = dynamic_cast<ExpressionInstruction*>(v->body->instructions[0])) {
			if (auto f = dynamic_cast<Function*>(ei->value)) {

				analyzer->enter_function(this);
				for (unsigned i = 0; i < arguments.size(); ++i) {
					analyzer->add_parameter(arguments[i].get(), v->type.argument(i));
				}
				f->will_take(analyzer, args, level - 1);

				analyzer->leave_function();

				if (captures.size()) {
					v->type = Type::closure(f->version_type(args), v->type.arguments(), this);
				} else {
					v->type = Type::fun(f->version_type(args), v->type.arguments(), this);
				}
			}
		}
	}
	return false;
}

void Function::set_version(const std::vector<Type>& args, int level) {
	// std::cout << "Function::set_version " << args << " " << level << std::endl;
	if (level == 1) {
		for (const auto& t : args) {
			if (t.is_placeholder()) return;
		}
		version = args;
		// Fill with defaultValues
		for (size_t i = version.size(); i < arguments.size(); ++i) {
			if (defaultValues.at(i)) {
				version.push_back(defaultValues.at(i)->type);
			}
		}
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

void Function::analyze_body(SemanticAnalyzer* analyzer, std::vector<Type> args, Version* version) {

	// std::cout << "Function::analyse_body(" << args << ")" << std::endl;

	captures.clear();
	analyzer->enter_function(this);
	current_version = version;

	// Prepare the placeholder return type for recursive functions
	if (captures.size()) {
		version->type = Type::closure(getReturnType(), args, this);
	} else {
		version->type = Type::fun(getReturnType(), args, this);
	}

	std::vector<Type> arg_types;
	for (unsigned i = 0; i < arguments.size(); ++i) {
		Type type = i < args.size() ? args.at(i) : (i < defaultValues.size() && defaultValues.at(i) != nullptr ? defaultValues.at(i)->type : Type::any());
		analyzer->add_parameter(arguments.at(i).get(), type);
		arg_types.push_back(type);
	}
	version->body->analyze(analyzer);
	if (captures.size()) {
		version->type = Type::closure(version->body->type, arg_types, this);
	} else {
		version->type = Type::fun(version->body->type, arg_types, this);
	}
	Type return_type;
	// std::cout << "version->body->type " << version->body->type << std::endl;
	// std::cout << "version->body->return_type " << version->body->return_type << std::endl;
	for (const auto& t : version->body->type._types) {
		if (dynamic_cast<const Placeholder_type*>(t.get()) == nullptr) {
			return_type += t;
		}
	}
	if (version->body->type.temporary) return_type.temporary = true;
	for (const auto& t : version->body->return_type._types) {
		if (dynamic_cast<const Placeholder_type*>(t.get()) == nullptr) {
			return_type += t;
		}
	}
	if (version->body->return_type.temporary) return_type.temporary = true;
	// Default version of the function, the return type must be any
	if (not return_type.is_void() and not is_main_function and version == default_version and generate_default_version) {
		return_type = Type::any();
	}
	// std::cout << "return_type " << return_type << std::endl;
	version->body->type = return_type;
	if (captures.size()) {
		version->type = Type::closure(return_type, arg_types, this);
	} else {
		version->type = Type::fun(return_type, arg_types, this);
	}
	// Re-analyse the recursive function to clean the placeholder types
	if (recursive) {
		version->body->analyze(analyzer);
	}

	vars = analyzer->get_local_vars();
	analyzer->leave_function();

	// std::cout << "function analysed body : " << version->type << std::endl;
}

int Function::capture(std::shared_ptr<SemanticVar> var) {
	// std::cout << "Function::capture " << var->name << std::endl;

	// Function become a closure
	default_version->type = Type::closure(default_version->type.return_type(), default_version->type.arguments(), this);
	for (auto& version : versions) {
		version.second->type = Type::closure(version.second->type.return_type(), version.second->type.arguments(), this);
	}
	type = Type::closure(type.return_type(), type.arguments(), this);

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

Type Function::version_type(std::vector<Type> version) const {
	// std::cout << "Function " << this << " ::version_type(" << version << ") " << std::endl;
	// Add default values types if needed
	for (size_t i = version.size(); i < arguments.size(); ++i) {
		if (defaultValues.at(i)) {
			version.push_back(defaultValues.at(i)->type);
		}
	}
	if (versions.find(version) != versions.end()) {
		return versions.at(version)->type;
	}
	return type;
}

void Function::must_return_any(SemanticAnalyzer*) {
	// std::cout << "Function " << name << " ::must_return_any()" << std::endl;
	generate_default_version = true;
}

Callable* Function::get_callable(SemanticAnalyzer*) const {
	auto callable = new Callable("<function>");
	int flags = default_version->body->throws ? Module::THROWS : 0;
	callable->add_version({ "<default>", default_version->type, default_version, {}, {}, nullptr, false, false, false, flags });
	for (const auto& version : versions) {
		int flags = version.second->body->throws ? Module::THROWS : 0;
		callable->add_version({ "<version>", version.second->type, version.second, {}, {}, nullptr, false, false, false, flags });
	}
	return callable;
}

Compiler::value Function::compile(Compiler& c) const {
	// std::cout << "Function::compile() " << this << " version " << version << " " << has_version << std::endl;
	((Function*) this)->compiler = &c;

	if (generate_default_version) {
		default_version->compile(c, true);
		return default_version->value;
	}
	if (is_main_function) {
		default_version->compile(c);
		return default_version->value;
	}
	if (has_version) {
		return compile_version(c, version);
	}
	// Returns a default function value
	return c.new_function(default_version->type);
}

Compiler::value Function::compile_version(Compiler& c, std::vector<Type> args) const {
	// std::cout << "Function " << name << "::compile_version(" << args << ")" << std::endl;
	// Fill with default arguments
	auto full_args = args;
	for (size_t i = version.size(); i < arguments.size(); ++i) {
		if (defaultValues.at(i)) {
			full_args.push_back(defaultValues.at(i)->type);
		}
	}
	if (versions.find(full_args) == versions.end()) {
		std::cout << "/!\\ Version " << args << " not found!" << std::endl;
		assert(false);
	}
	auto version = versions.at(full_args);
	// Compile version if needed
	version->compile(c, true);
	return version->value;
}

Compiler::value Function::compile_default_version(Compiler& c) const {
	// std::cout << "Function " << name << "::compile_default_version " << std::endl;
	default_version->compile(c, true, false);
	return default_version->value;
}

llvm::BasicBlock* Function::get_landing_pad(const Compiler& c) {
	// std::cout << "get_landing_pad " << current_version->type << " " << current_version->landing_pad << std::endl;
	if (current_version->landing_pad == nullptr) {
		current_version->catch_block = llvm::BasicBlock::Create(c.getContext(), "catch", c.F);
		auto savedIP = c.builder.saveAndClearIP();
		current_version->landing_pad = llvm::BasicBlock::Create(c.getContext(), "lpad", c.F);
		c.builder.SetInsertPoint(current_version->landing_pad);
		auto catchAllSelector = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(c.getContext()));
		auto landingPadInst = c.builder.CreateLandingPad(llvm::StructType::get(llvm::Type::getInt64Ty(c.getContext()), llvm::Type::getInt32Ty(c.getContext())), 1);
		auto LPadExn = c.builder.CreateExtractValue(landingPadInst, 0);
		current_version->exception_slot = c.CreateEntryBlockAlloca("exn.slot", llvm::Type::getInt64Ty(c.getContext()));
		current_version->exception_line_slot = c.CreateEntryBlockAlloca("exnline.slot", llvm::Type::getInt64Ty(c.getContext()));
		c.builder.CreateStore(LPadExn, current_version->exception_slot);
		c.builder.CreateStore(c.new_long(c.exception_line.top()).v, current_version->exception_line_slot);
		landingPadInst->addClause(catchAllSelector);
		auto catcher = c.find_catcher();
		if (catcher) {
			c.builder.CreateBr(catcher->handler);
		} else {
			c.builder.CreateBr(current_version->catch_block);
		}
		c.builder.restoreIP(savedIP);
		// std::cout << "Landing pad of " << current_version->type << " created! " << current_version->landing_pad << std::endl;
	}
	return current_version->landing_pad;
}

void Function::Version::create_function(Compiler& c) {
	if (f) return;

	std::vector<llvm::Type*> args;
	if (parent->captures.size()) {
		args.push_back(Type::any().llvm_type(c)); // first arg is the function pointer
	}
	for (auto& t : this->type.arguments()) {
		args.push_back(t.llvm_type(c));
	}

	// const int id = id_counter++;
	auto llvm_return_type = this->type.return_type().llvm_type(c);
	auto function_type = llvm::FunctionType::get(llvm_return_type, args, false);
	auto fun_name = parent->is_main_function ? "main" : parent->name;
	f = llvm::Function::Create(function_type, llvm::Function::InternalLinkage, fun_name, c.program->module);

	if (body->throws) {
		auto personalityfn = c.program->module->getFunction("__gxx_personality_v0");
		if (!personalityfn) {
			personalityfn = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(c.getContext()), true), llvm::Function::ExternalLinkage, "__gxx_personality_v0", c.program->module);
		}
		f->setPersonalityFn(personalityfn);
	}

	block = llvm::BasicBlock::Create(c.getContext(), "start", f);
}

void Function::Version::compile(Compiler& c, bool create_value, bool compile_body) {
	// std::cout << "Function::Version " << parent->name << "::compile(" << type << ", create_value=" << create_value << ")" << std::endl;

	if (not is_compiled()) {
		
		this->parent->current_version = this;

		std::vector<llvm::Type*> args;
		if (parent->captures.size()) {
			args.push_back(Type::any().llvm_type(c)); // first arg is the function pointer
		}
		for (auto& t : this->type.arguments()) {
			args.push_back(t.llvm_type(c));
		}
		// Create the llvm function
		create_function(c);

		c.enter_function(f, parent->captures.size() > 0, parent);

		c.builder.SetInsertPoint(block);

		// Create arguments
		unsigned index = 0;
		int offset = parent->captures.size() ? -1 : 0;
		for (auto& arg : f->args()) {
			if (offset + index < parent->arguments.size()) {
				const auto name = parent->arguments.at(offset + index)->content;
				const auto type = this->type.arguments().at(offset + index).not_temporary();
				arg.setName(name);
				auto var_type = type.is_mpz_ptr() ? Type::mpz() : type;
				auto var = c.create_entry(name, var_type);
				if (type.is_mpz_ptr()) {
					c.insn_store(var, c.insn_load({&arg, type}));
				} else {
					c.insn_store(var, {&arg, type});
				}
				c.arguments.top()[name] = var;
				// c.arguments.top()[name] = {&arg, type};
			}
			index++;
		}

		if (compile_body) {
			body->compile(c);
		} else {
			parent->compile_return(c, {});
		}

		// Catch block
		if (landing_pad != nullptr) {
			c.builder.SetInsertPoint(catch_block);
			// TODO : here, we delete all the function variables, even some variables that may already be destroyed
			// TODO : To fix, create a landing pad for every call that can throw
			c.delete_function_variables();
			Compiler::value exception = {c.builder.CreateLoad(exception_slot), Type::long_()};
			Compiler::value exception_line = {c.builder.CreateLoad(exception_line_slot), Type::long_()};
			auto function_name = c.new_const_string(c.fun->name, "fun");
			c.insn_call({}, {exception, function_name, exception_line}, "System.throw.1");
			c.fun->compile_return(c, {});
		}

		if (!parent->is_main_function) {
			c.leave_function();
			// Create a function : 1 op
			c.inc_ops(1);
		} else {
			c.instructions_debug << "}" << std::endl;
		}
		llvm::verifyFunction(*f);
	}

	if (create_value) {
		if (parent->captures.size()) {
			std::vector<Compiler::value> captures;
			if (!parent->is_main_function) {
				for (const auto& cap : parent->captures) {
					Compiler::value jit_cap;
					if (cap->scope == VarScope::LOCAL) {
						auto f = dynamic_cast<Function*>(cap->value);
						if (f) {
							if (cap->has_version) {
								jit_cap = f->compile_version(c, cap->version);
							} else {
								jit_cap = f->compile_default_version(c);
							}
						} else {
							if (cap->type().is_mpz_ptr()) {
								jit_cap = c.get_var(cap->name);
							} else {
								jit_cap = c.insn_load(c.get_var(cap->name));
							}
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
					captures.push_back(jit_cap);
				}
			}
			value = c.new_closure(f, type, captures);
		} else {
			value = c.new_function(f, type);
		}
	}
	// std::cout << "Function '" << parent->name << "' compiled: " << value.v << std::endl;
}

void Function::compile_return(const Compiler& c, Compiler::value v, bool delete_variables) const {
	c.assert_value_ok(v);
	// Delete temporary mpz arguments
	for (size_t i = 0; i < current_version->type.arguments().size(); ++i) {
		const auto& name = arguments.at(i)->content;
		const auto& arg = ((Compiler&) c).arguments.top().at(name);
		if (current_version->type.argument(i) == Type::tmp_mpz_ptr()) {
			c.insn_delete_mpz(arg);
		}
	}
	// Delete function variables if needed
	if (delete_variables) {
		c.delete_function_variables();
	}
	// Return the value
	if (current_version->type.return_type().is_void()) {
		c.insn_return_void();
	} else {
		auto return_type = c.fun->getReturnType().fold();
		if (v.t.is_void()) {
			if (return_type.is_bool()) v = c.new_bool(false);
			else if (return_type.is_real()) v = c.new_real(0);
			else v = c.new_integer(0);
		}
		if (return_type.is_any()) {
			v = c.insn_convert(v, return_type);
		}
		c.assert_value_ok(v);
		c.insn_return(v);
	}
}

Value* Function::clone() const {
	auto f = new Function();
	f->lambda = lambda;
	f->name = name;
	f->body = (Block*) body->clone();
	f->recursive = recursive;
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
	f->default_values_count = default_values_count;
	for (const auto& v : versions) {
		auto v2 = new Version();
		v2->parent = f;
		v2->body = (Block*) v.second->body->clone();
		v2->type = v.second->type;
		f->versions.insert({v.first, v2});
	}
	return f;
}

}
