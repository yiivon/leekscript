#include "FunctionVersion.hpp"
#include "../Compiler.hpp"
#include "../../type/Type.hpp"
#include "../value/Function.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/Context.hpp"
#include "../resolver/File.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../vm/Program.hpp"
#include "../../colors.h"
#include "../../type/Placeholder_type.hpp"
#include "../../type/Compound_type.hpp"
#include "../../type/Function_type.hpp"
#include "Variable.hpp"

namespace ls {

FunctionVersion::FunctionVersion(std::unique_ptr<Block> body) : body(std::move(body)), type(Type::void_) {}

bool FunctionVersion::is_compiled() const {
	return fun.v != nullptr;
}

void FunctionVersion::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << captures[c]->name << " " << captures[c]->type;
		}
		os << "] ";
	}
	if (parent->arguments.size() != 1) {
		os << "(";
	}
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		if (i > 0) os << ", ";
		os << parent->arguments.at(i)->content;
		if (debug)
			os << " " << this->type->arguments().at(i);

		if (parent->defaultValues.at(i) != nullptr) {
			os << " = ";
			parent->defaultValues.at(i)->print(os);
		}
	}
	if (parent->arguments.size() != 1) {
		os << ")";
	}
	if (debug and placeholder_type) {
		os << " " << placeholder_type;
	}
	if (recursive) {
		os << BLUE_BOLD << " recursive" << END_COLOR;
	}
	if (this->body->throws) {
		os << BLUE_BOLD << " throws" << END_COLOR;
	}
	os << " => ";
	body->print(os, indent, debug, condensed);

	if (debug) {
		// os << " [" << parent->versions.size() << " versions, " << std::boolalpha << parent->has_version << "]";
		// os << "<";
		// int i = 0;
		// for (const auto& v : parent->versions) {
		// 	if (i > 0) os << ", ";
		// 	if (v.second == this) os << "$";
		// 	os << v.first << " => " << v.second->type->return_type();
		// 	i++;
		// }
		// os << ">";
	}
	if (debug) {
		os << " : " << type->return_type();
	}
}

const Type* FunctionVersion::getReturnType() {
	// std::cout << "getReturnType() " << type << std::endl;
	if (type->is_void()) {
		if (!placeholder_type) {
			placeholder_type = Type::generate_new_placeholder_type();
		}
		return placeholder_type;
	} else {
		return type->return_type();
	}
}

void FunctionVersion::pre_analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {
	if (pre_analyzed) return;
	pre_analyzed = true;
	analyzer->enter_function((FunctionVersion*) this);
	// Create arguments
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		auto type = i < args.size() ? args.at(i) : (i < parent->defaultValues.size() && parent->defaultValues.at(i) != nullptr ? parent->defaultValues.at(i)->type : Type::any);
		auto name = parent->arguments.at(i)->content;
		auto arg = new Variable(name, VarScope::PARAMETER, type, i, nullptr, (FunctionVersion*) this, nullptr);
		arguments.insert({ name, arg });
	}
	body->pre_analyze(analyzer);
	analyzer->leave_function();
}

void FunctionVersion::analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {
	// std::cout << "Function::analyse_body(" << args << ")" << std::endl;

	captures.clear();
	parent->current_version = this;
	analyzer->enter_function((FunctionVersion*) this);

	// Prepare the placeholder return type for recursive functions
	type = Type::fun(getReturnType(), args, parent)->pointer();

	std::vector<const Type*> arg_types;
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		auto type = i < args.size() ? args.at(i) : (i < parent->defaultValues.size() && parent->defaultValues.at(i) != nullptr ? parent->defaultValues.at(i)->type : Type::any);
		arg_types.push_back(type);
	}

	body->analyze(analyzer);
	
	auto return_type = Type::void_;
	// std::cout << "body->type " << body->type << std::endl;
	// std::cout << "body->return_type " << body->return_type << std::endl;
	if (auto c = dynamic_cast<const Compound_type*>(body->type)) {
		for (const auto& t : c->types) {
			if (t != placeholder_type) {
				return_type = return_type->operator + (t);
			}
		}
	} else {
		return_type = body->type;
	}
	if (auto c = dynamic_cast<const Compound_type*>(body->return_type)) {
		for (const auto& t : c->types) {
			if (t != placeholder_type) {
				return_type = return_type->operator + (t);
			}
		}
	} else {
		return_type = return_type->operator + (body->return_type);
	}
	if (body->type->temporary or body->return_type->temporary) return_type = return_type->add_temporary();
	// std::cout << "return type = " << return_type << std::endl;

	// Default version of the function, the return type must be any
	if (not return_type->is_void() and not parent->is_main_function and this == parent->default_version and parent->generate_default_version) {
		return_type = Type::any;
	}
	if (return_type->is_function()) {
		return_type = return_type->pointer();
	}
	// std::cout << "return_type " << return_type << std::endl;
	body->type = return_type;
	if (captures.size()) {
		type = Type::closure(return_type, arg_types, parent);
	} else {
		type = Type::fun(return_type, arg_types, parent)->pointer();
	}
	
	// Re-analyse the recursive function to clean the placeholder types
	if (recursive) {
		body->analyze(analyzer);
	}

	analyzer->leave_function();

	// std::cout << "function analysed body : " << version->type << std::endl;
}

int FunctionVersion::capture(SemanticAnalyzer* analyzer, Variable* var) {
	// std::cout << "Function::capture " << var->name << std::endl;

	type = Type::closure(getReturnType(), type->arguments());

	for (size_t i = 0; i < captures.size(); ++i) {
		if (captures[i]->name == var->name)
			return i;
	}
	var = new Variable(*var);
	captures.push_back(var);

	if (var->function->parent != parent->parent) {
		// std::cout << "Capture by parent" << std::endl;
		auto new_var = new Variable(*var);
		// std::cout << "parents versions " << parent->parent->versions.size() << std::endl;
		auto parent_version = parent->parent->versions.size() ? parent->parent->versions.begin()->second : parent->parent->default_version;
		new_var->index = parent_version->capture(analyzer, new_var);
		var->scope = VarScope::CAPTURE;
		var->parent_index = new_var->index;
	}
	return captures.size() - 1;
}

void FunctionVersion::create_function(Compiler& c) {
	if (fun.v) return;

	std::vector<const Type*> args;
	if (captures.size()) {
		args.push_back(Type::any); // first arg is the function pointer
	}
	for (auto& t : this->type->arguments()) {
		args.push_back(t);
	}
	auto function_type = Type::fun(type->return_type(), args);
	auto fun_name = parent->is_main_function ? "main" : parent->name;
	auto f = llvm::Function::Create((llvm::FunctionType*) function_type->llvm(c), llvm::Function::InternalLinkage, fun_name, c.program->module);
	fun = { f, function_type->pointer() };
	assert(c.check_value(fun));

	if (body->throws) {
		auto personalityfn = c.program->module->getFunction("__gxx_personality_v0");
		if (!personalityfn) {
			personalityfn = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(c.getContext()), true), llvm::Function::ExternalLinkage, "__gxx_personality_v0", c.program->module);
		}
		f->setPersonalityFn(personalityfn);
	}
	block = llvm::BasicBlock::Create(c.getContext(), "start", f);
}

Compiler::value FunctionVersion::compile(Compiler& c, bool compile_body) {
	// std::cout << "Function::Version " << parent->name << "::compile(" << type << ", compile_body=" << compile_body << ")" << std::endl;
	// std::cout << "Captures : " << captures.size() << " : ";
	// for (const auto& c : captures) std::cout << c->name << " " << c->type << " " << (int)c->scope << " ";
	// std::cout << std::endl;

	if (not is_compiled()) {

		for (const auto& cap : captures) {
			if (cap->scope == VarScope::LOCAL or cap->scope == VarScope::PARAMETER) {
				c.convert_var_to_poly(cap->name);
			}
		}
		std::vector<llvm::Type*> args;
		if (captures.size()) {
			args.push_back(Type::any->llvm(c)); // first arg is the function pointer
		}
		for (auto& t : this->type->arguments()) {
			args.push_back(t->llvm(c));
		}
		// Create the llvm function
		create_function(c);

		c.enter_function((llvm::Function*) fun.v, captures.size() > 0, this);

		c.builder.SetInsertPoint(block);

		// Declare context vars
		if (parent->is_main_function and c.vm->context) {
			for (const auto& var : c.vm->context->vars) {
				// std::cout << "Main function compile context var " << var.first << std::endl;
				c.add_external_var(var.first, var.second.type);
			}
		}

		// Create arguments
		unsigned index = 0;
		int offset = captures.size() ? -1 : 0;
		for (auto& arg : ((llvm::Function*) fun.v)->args()) {
			if (index == 0 && captures.size()) {
				arg.setName("closure");
			} else if (offset + index < parent->arguments.size()) {
				const auto name = parent->arguments.at(offset + index)->content;
				const auto type = this->type->arguments().at(offset + index)->not_temporary();
				arg.setName(name);
				auto var_type = type->is_mpz_ptr() ? Type::mpz : type;
				auto var = c.create_entry(name, var_type);
				if (type->is_mpz_ptr()) {
					c.insn_store(var, c.insn_load({&arg, type}));
				} else {
					if (this->type->arguments().at(offset + index)->temporary) {
						Compiler::value a = {&arg, type};
						c.insn_inc_refs(a);
						c.insn_store(var, a);
					} else {
						c.insn_store(var, {&arg, type});
					}
				}
				c.arguments.top()[name] = var;
			}
			index++;
		}

		if (compile_body) {
			body->compile(c);
		} else {
			compile_return(c, {});
		}

		if (!parent->is_main_function) {
			c.leave_function();
			// Create a function : 1 op
			c.inc_ops(1);
		}
		llvm::verifyFunction(*((llvm::Function*) fun.v));
	}

	if (captures.size()) {
		std::vector<Compiler::value> captures;
		if (!parent->is_main_function) {
			for (const auto& cap : this->captures) {
				Compiler::value jit_cap;
				if (cap->scope == VarScope::LOCAL) {
					auto f = dynamic_cast<Function*>(cap->value);
					if (f) {
						jit_cap = f->compile_version(c, cap->version);
					} else {
						if (cap->type->is_mpz_ptr()) {
							jit_cap = c.get_var(cap->name);
						} else {
							jit_cap = c.insn_load(c.get_var(cap->name));
						}
					}
				} else if (cap->scope == VarScope::CAPTURE) {
					jit_cap = c.insn_get_capture(cap->parent_index, cap->type);
				} else {
					jit_cap = c.insn_load(c.insn_get_argument(cap->name));
				}
				assert(jit_cap.t->is_polymorphic());
				captures.push_back(jit_cap);
			}
		}
		value = c.new_closure(fun, captures);
	} else {
		// if (fun.t->is_pointer()) {
			value = fun;
		// } else {
			// value = { c.builder.CreatePointerCast(fun.v, fun.t->pointer()->llvm(c)), fun.t->pointer() };
		// }
	}
	// std::cout << "Function '" << parent->name << "' compiled: " << value.t << std::endl;
	return value;
}

void FunctionVersion::compile_return(const Compiler& c, Compiler::value v, bool delete_variables) const {
	assert(c.check_value(v));
	// Delete temporary mpz arguments
	for (size_t i = 0; i < type->arguments().size(); ++i) {
		const auto& name = parent->arguments.at(i)->content;
		const auto& arg = ((Compiler&) c).arguments.top().at(name);
		if (type->argument(i) == Type::tmp_mpz_ptr) {
			// std::cout << "delete tmp arg " << name << " " << type->argument(i) << std::endl;
			c.insn_delete_mpz(arg);
		} else if (type->argument(i)->temporary) {
			c.insn_delete(c.insn_load(arg));
		}
	}
	// Delete function variables if needed
	if (delete_variables) {
		c.delete_function_variables();
	}
	// Return the value
	if (type->return_type()->is_void()) {
		c.insn_return_void();
	} else {
		auto return_type = ((FunctionVersion*) this)->getReturnType()->fold();
		// std::cout << "return type " << return_type << std::endl;
		if (v.t->is_void()) {
			if (return_type->is_bool()) v = c.new_bool(false);
			else if (return_type->is_real()) v = c.new_real(0);
			else if (return_type->is_long()) v = c.new_long(0);
			// else if (return_type->is_raw_function()) v = c.new_null_pointer();
			else v = c.new_integer(0);
		}
		if (return_type->is_any()) {
			v = c.insn_convert(v, return_type);
		}
		// if (return_type->is_function()) {
		// 	std::cout << "return type function " << std::endl;
		// 	v = { c.builder.CreatePointerCast(v.v, return_type->pointer()->llvm(c)), return_type->pointer() };
		// }
		assert(c.check_value(v));
		c.insn_return(v);
	}
}

llvm::BasicBlock* FunctionVersion::get_landing_pad(const Compiler& c) {
	auto catch_block = llvm::BasicBlock::Create(c.getContext(), "catch", c.F);
	auto savedIP = c.builder.saveAndClearIP();
	auto landing_pad = llvm::BasicBlock::Create(c.getContext(), "lpad", c.F);
	c.builder.SetInsertPoint(landing_pad);
	auto catchAllSelector = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(c.getContext()));
	auto landingPadInst = c.builder.CreateLandingPad(llvm::StructType::get(llvm::Type::getInt64Ty(c.getContext()), llvm::Type::getInt32Ty(c.getContext())), 1);
	auto LPadExn = c.builder.CreateExtractValue(landingPadInst, 0);
	auto exception_slot = c.CreateEntryBlockAlloca("exn.slot", llvm::Type::getInt64Ty(c.getContext()));
	auto exception_line_slot = c.CreateEntryBlockAlloca("exnline.slot", llvm::Type::getInt64Ty(c.getContext()));
	c.builder.CreateStore(LPadExn, exception_slot);
	c.builder.CreateStore(c.new_long(c.exception_line.top()).v, exception_line_slot);
	landingPadInst->addClause(catchAllSelector);
	auto catcher = c.find_catcher();
	if (catcher) {
		c.builder.CreateBr(catcher->handler);
	} else {
		// Catch block
		auto savedIPc = c.builder.saveAndClearIP();
		c.builder.SetInsertPoint(catch_block);
		c.delete_function_variables();
		Compiler::value exception = {c.builder.CreateLoad(exception_slot), Type::long_};
		Compiler::value exception_line = {c.builder.CreateLoad(exception_line_slot), Type::long_};
		auto file = c.new_const_string(parent->token->location.file->path);
		auto function_name = c.new_const_string(parent->name);
		c.insn_call(Type::void_, {exception, file, function_name, exception_line}, "System.throw.1");
		c.fun->compile_return(c, {});
		c.builder.restoreIP(savedIPc);

		c.builder.CreateBr(catch_block);
	}
	c.builder.restoreIP(savedIP);
	return landing_pad;
}

}