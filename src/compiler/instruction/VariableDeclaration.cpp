#include "../../compiler/instruction/VariableDeclaration.hpp"

#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"
#include "../value/Reference.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
	return_value = false;
	type = Type::VALUE;
}

VariableDeclaration::~VariableDeclaration() {
	for (auto ex : expressions) {
		delete ex;
	}
}

void VariableDeclaration::print(ostream& os, bool debug) const {

	os << (global ? "global " : "let ");

	for (unsigned i = 0; i < variables.size(); ++i) {
		os << variables.at(i)->content;
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
	if (expressions.size() > 0) {
		os << " = ";
	}
	for (unsigned i = 0; i < expressions.size(); ++i) {
		expressions.at(i)->print(os, debug);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	for (unsigned i = 0; i < variables.size(); ++i) {

		Token* var = variables[i];
		Type type = Type::NEUTRAL;
		Value* value = nullptr;

		SemanticVar* v = analyser->add_var(var, Type::FUNCTION, value, this);

		if (i < expressions.size()) {
			expressions[i]->analyse(analyser, Type::UNKNOWN);
			v->type = expressions[i]->type;
			v->value = expressions[i];
		}

		if (v->type == Type::VOID) {
			analyser->add_error({SemanticException::Type::CANT_ASSIGN_VOID, var->line, var->content});
		}

		vars.insert(pair<string, SemanticVar*>(var->content, v));
		vars.at(var->content)->type = v->type;
	}
	this->return_value = return_value;

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

jit_value_t VariableDeclaration::compile(Compiler& c) const {

	for (unsigned i = 0; i < variables.size(); ++i) {

		std::string name = variables.at(i)->content;
		SemanticVar* v = vars.at(name);

		if (i < expressions.size()) {

			Value* ex = expressions[i];

			jit_value_t var = jit_value_create(c.F, VM::get_jit_type(v->type));
			c.add_var(name, var, v->type, false);

			jit_value_t val;
			if (Reference* ref = dynamic_cast<Reference*>(ex)) {
				val = c.get_var(ref->variable->content).value;
				c.add_var(name, val, v->type, true);
			} else {
				val = ex->compile(c);
				c.set_var_type(name, ex->type);
				jit_insn_store(c.F, var, val);
				if (expressions.at(i)->type.must_manage_memory()) {
					VM::inc_refs(c.F, val);
				}
			}
			if (i == expressions.size() - 1) {
				if (v->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, val, type);
				}
				return val;
			}
		} else {

			jit_value_t var = jit_value_create(c.F, JIT_POINTER);
			c.add_var(name, var, Type::NULLL, false);

			jit_value_t val = VM::create_null(c.F);
			jit_insn_store(c.F, var, val);
			VM::inc_refs(c.F, val);
		}
	}
	return VM::create_null(c.F);
}

}
