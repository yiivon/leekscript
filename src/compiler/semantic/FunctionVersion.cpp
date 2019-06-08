#include "FunctionVersion.hpp"
#include "../Compiler.hpp"
#include "../../type/Type.hpp"
#include "../value/Function.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/Context.hpp"
#include "../resolver/File.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../vm/Program.hpp"

namespace ls {

FunctionVersion::FunctionVersion() : type(Type::void_) {}

bool FunctionVersion::is_compiled() const {
	return f != nullptr;
}

void FunctionVersion::create_function(Compiler& c) {
	if (f) return;

	std::vector<llvm::Type*> args;
	if (parent->captures.size()) {
		args.push_back(Type::any->llvm(c)); // first arg is the function pointer
	}
	for (auto& t : this->type->arguments()) {
		args.push_back(t->llvm(c));
	}

	// const int id = id_counter++;
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
		
		parent->current_version = this;

		std::vector<llvm::Type*> args;
		if (parent->captures.size()) {
			args.push_back(Type::any->llvm(c)); // first arg is the function pointer
		}
		for (auto& t : this->type->arguments()) {
			args.push_back(t->llvm(c));
		}
		// Create the llvm function
		create_function(c);

		c.enter_function(f, parent->captures.size() > 0, parent);

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
		int offset = parent->captures.size() ? -1 : 0;
		for (auto& arg : f->args()) {
			if (offset + index < parent->arguments.size()) {
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
			parent->compile_return(c, {});
		}

		// Catch block
		if (landing_pad != nullptr) {
			c.builder.SetInsertPoint(catch_block);
			// TODO : here, we delete all the function variables, even some variables that may already be destroyed
			// TODO : To fix, create a landing pad for every call that can throw
			c.delete_function_variables();
			Compiler::value exception = {c.builder.CreateLoad(exception_slot), Type::long_};
			Compiler::value exception_line = {c.builder.CreateLoad(exception_line_slot), Type::long_};
			auto file = c.new_const_string(c.fun->token->location.file->path);
			auto function_name = c.new_const_string(c.fun->name);
			c.insn_call(Type::void_, {exception, file, function_name, exception_line}, "System.throw.1");
			c.fun->compile_return(c, {});
		}

		if (!parent->is_main_function) {
			c.leave_function();
			// Create a function : 1 op
			c.inc_ops(1);
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

}