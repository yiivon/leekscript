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
#include "Variable.hpp"

namespace ls {

FunctionVersion::FunctionVersion(std::unique_ptr<Block> body) : body(std::move(body)), type(Type::void_) {}

bool FunctionVersion::is_compiled() const {
	return f != nullptr;
}

void FunctionVersion::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (parent->captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < parent->captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << parent->captures[c]->name << " " << parent->captures[c]->type;
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
	if (type->return_type()->is_void()) {
		if (!placeholder_type) {
			placeholder_type = Type::generate_new_placeholder_type();
		}
		return placeholder_type;
	} else {
		return type->return_type();
	}
}

void FunctionVersion::pre_analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {
	// Create arguments
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		auto type = i < args.size() ? args.at(i) : (i < parent->defaultValues.size() && parent->defaultValues.at(i) != nullptr ? parent->defaultValues.at(i)->type : Type::any);
		auto name = parent->arguments.at(i)->content;
		auto arg = new Variable(name, VarScope::PARAMETER, type, i, nullptr, (FunctionVersion*) this, nullptr);
		arguments.insert({ name, arg });
	}
	body->pre_analyze(analyzer);
}

void FunctionVersion::analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {

	// std::cout << "Function::analyse_body(" << args << ")" << std::endl;

	parent->captures.clear();
	parent->current_version = this;
	analyzer->enter_function((FunctionVersion*) this);

	// Prepare the placeholder return type for recursive functions
	if (parent->captures.size() or parent->is_closure) {
		type = Type::closure(getReturnType(), args, parent);
	} else {
		type = Type::fun(getReturnType(), args, parent);
	}

	std::vector<const Type*> arg_types;
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		auto type = i < args.size() ? args.at(i) : (i < parent->defaultValues.size() && parent->defaultValues.at(i) != nullptr ? parent->defaultValues.at(i)->type : Type::any);
		arg_types.push_back(type);
	}

	body->analyze(analyzer);
	if (parent->captures.size() or parent->is_closure) {
		type = Type::closure(body->type, arg_types, parent);
	} else {
		type = Type::fun(body->type, arg_types, parent);
	}
	auto return_type = Type::void_;
	// std::cout << "version->body->type " << body->type << std::endl;
	// std::cout << "version->body->return_type " << body->return_type << std::endl;
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
	// std::cout << "return_type " << return_type << std::endl;
	body->type = return_type;
	if (parent->captures.size() or parent->is_closure) {
		type = Type::closure(return_type, arg_types, parent);
	} else {
		type = Type::fun(return_type, arg_types, parent);
	}
	// Re-analyse the recursive function to clean the placeholder types
	if (recursive) {
		body->analyze(analyzer);
	}

	analyzer->leave_function();

	// std::cout << "function analysed body : " << version->type << std::endl;
}

void FunctionVersion::create_function(Compiler& c) {
	if (f) return;

	std::vector<llvm::Type*> args;
	if (parent->captures.size() or parent->is_closure) {
		args.push_back(Type::any->llvm(c)); // first arg is the function pointer
	}
	for (auto& t : this->type->arguments()) {
		args.push_back(t->llvm(c));
	}

	auto llvm_return_type = this->type->return_type()->llvm(c);
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

void FunctionVersion::compile(Compiler& c, bool create_value, bool compile_body) {
	// std::cout << "Function::Version " << parent->name << "::compile(" << type << ", create_value=" << create_value << ")" << std::endl;

	if (not is_compiled()) {

		std::vector<llvm::Type*> args;
		if (parent->captures.size() or parent->is_closure) {
			args.push_back(Type::any->llvm(c)); // first arg is the function pointer
		}
		for (auto& t : this->type->arguments()) {
			args.push_back(t->llvm(c));
		}
		// Create the llvm function
		create_function(c);

		c.enter_function(f, parent->captures.size() > 0 or parent->is_closure, this);

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
		int offset = parent->captures.size() or parent->is_closure ? -1 : 0;
		for (auto& arg : f->args()) {
			if (index == 0 && (parent->captures.size() or parent->is_closure)) {
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
				// c.arguments.top()[name] = {&arg, type};
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
		llvm::verifyFunction(*f);
	}

	if (create_value) {
		if (parent->captures.size() or parent->is_closure) {
			std::vector<Compiler::value> captures;
			if (!parent->is_main_function) {
				for (const auto& cap : parent->captures) {
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
			value = c.new_closure(f, type, captures);
		} else {
			value = c.new_function(f, type);
		}
	}
	// std::cout << "Function '" << parent->name << "' compiled: " << value.v << std::endl;
}

void FunctionVersion::compile_return(const Compiler& c, Compiler::value v, bool delete_variables) const {
	c.assert_value_ok(v);
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
		if (v.t->is_void()) {
			if (return_type->is_bool()) v = c.new_bool(false);
			else if (return_type->is_real()) v = c.new_real(0);
			else v = c.new_integer(0);
		}
		if (return_type->is_any()) {
			v = c.insn_convert(v, return_type);
		}
		c.assert_value_ok(v);
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