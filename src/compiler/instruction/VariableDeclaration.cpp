#include "VariableDeclaration.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
	constant = false;
}

VariableDeclaration::~VariableDeclaration() {
	for (const auto& ex : expressions) {
		delete ex;
	}
}

void VariableDeclaration::print(ostream& os, int indent, bool debug) const {

	os << (global ? "global " : (constant ? "let " : "var "));

	for (unsigned i = 0; i < variables.size(); ++i) {
		auto name = variables.at(i)->content;
		os << name;
		if (debug && vars.find(name) != vars.end()) {
			os << " " << vars.at(name)->type;
		}
		if (expressions[i] != nullptr) {
			os << " = ";
			expressions.at(i)->print(os, indent, debug);
		}
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
}

Location VariableDeclaration::location() const {
	auto end = variables.size() > expressions.size() ? variables.back()->location.end : expressions.back()->location().end;
	return {keyword->location.start, end};
}

void VariableDeclaration::analyse_global_functions(SemanticAnalyser* analyser) {
	if (global && function) {
		auto var = variables.at(0);
		auto expr = expressions.at(0);
		auto v = analyser->add_var(var.get(), Type::FUNCTION_P, expr, this);
		vars.insert({var->content, v});
	}
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	type = {};

	vars.clear();
	for (unsigned i = 0; i < variables.size(); ++i) {

		auto& var = variables.at(i);
		auto v = analyser->add_var(var.get(), Type::POINTER, expressions.at(i), this);
		if (v == nullptr) {
			continue;
		}

		if (expressions[i] != nullptr) {
			if (Function* f = dynamic_cast<Function*>(expressions[i])) {
				f->name = var->content;
				f->file = VM::current()->file_name;
			}
			expressions[i]->analyse(analyser);
			v->type = expressions[i]->type;
			v->type.temporary = false;
			v->type.constant = constant;
			v->value = expressions[i];
		} else {
			v->type = Type::NULLL;
			v->type.constant = constant;
		}
		if (v->type ._types.size() == 0) {
			analyser->add_error({SemanticError::Type::CANT_ASSIGN_VOID, location(), var->location, {var->content}});
		}
		vars.insert({var->content, v});
	}
}

Compiler::value VariableDeclaration::compile(Compiler& c) const {

	for (unsigned i = 0; i < variables.size(); ++i) {

		std::string name = variables[i]->content;
		auto v = vars.at(name);

		if (expressions[i] != nullptr) {

			Value* ex = expressions[i];
			auto var = c.create_and_add_var(name, v->type);

			if (Function* f = dynamic_cast<Function*>(ex)) {
				if (v->has_version && f->versions.find(v->version) != f->versions.end()) {
					c.insn_store(var, c.new_pointer((void*) f->versions.at(v->version)->function));
				} else {
					c.insn_store(var, c.new_pointer((void*) f->default_version->function));
				}
			}
			auto val = ex->compile(c);
			ex->compile_end(c);

			// c.insn_call({}, {val}, +[](__mpz_struct mpz) {
			// 	std::cout << "mpz store alloc = " << mpz._mp_alloc << std::endl;
			// 	std::cout << "mpz store size = " << mpz._mp_size << std::endl;
			// 	std::cout << "mpz store d = " << mpz._mp_d << std::endl;
			// });

			if (!val.t.reference) {
				val = c.insn_move_inc(val);
			}
			c.set_var_type(name, ex->type.not_temporary());
			c.add_function_var(var);
			c.insn_store(var, val);

		} else {
			auto var = c.create_and_add_var(name, Type::NULLL);
			auto val = c.new_null();
			c.insn_store(var, val);
		}
	}
	return {nullptr, {}};
}

Instruction* VariableDeclaration::clone() const {
	auto vd = new VariableDeclaration();
	vd->keyword = keyword;
	vd->global = global;
	vd->constant = constant;
	vd->function = function;
	for (const auto& v : variables) {
		vd->variables.push_back(v);
	}
	for (const auto& v : expressions) {
		vd->expressions.push_back(v ? v->clone() : nullptr);
	}
	return vd;
}

}
