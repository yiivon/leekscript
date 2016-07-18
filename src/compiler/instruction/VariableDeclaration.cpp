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

void VariableDeclaration::print(ostream& os) const {

	os << (global ? "global " : "let ");

	for (unsigned i = 0; i < variables.size(); ++i) {
		os << variables.at(i);
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
	if (expressions.size() > 0) {
		os << " = ";
	}
	for (unsigned i = 0; i < expressions.size(); ++i) {
		expressions.at(i)->print(os);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	type = Type::VALUE;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		expressions[i]->analyse(analyser, Type::NEUTRAL);
		if (i == expressions.size() - 1 and expressions[i]->type.nature == Nature::POINTER) {
			type = Type::POINTER;
		}
	}
	for (unsigned i = 0; i < variables.size(); ++i) {

		Token* var = variables[i];
		Type type = Type::NEUTRAL;
		Value* value = nullptr;
		if (i < expressions.size()) {
			type = expressions[i]->type;
			value = expressions[i];
		}

		if (type == Type::VOID) {
			throw SemanticException(SemanticException::Type::CANT_ASSIGN_VOID, var);
		}

		// Global Variable already defined ? We don't define another time
//		if (vars.find(var->content) == vars.end()) {
			SemanticVar* v = analyser->add_var(var, type, value);
			vars.insert(pair<string, SemanticVar*>(var->content, v));
//		}
		vars.at(var->content)->type = type;
	}
	this->return_value = return_value;
}

jit_value_t VariableDeclaration::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	for (unsigned i = 0; i < variables.size(); ++i) {

		std::string name = variables.at(i)->content;
		SemanticVar* v = vars.at(name);

		if (i < expressions.size()) {

			Value* ex = expressions[i];

			jit_value_t var = jit_value_create(F, VM::get_jit_type(ex->type));
			c.add_var(name, var, v->type, false);

			jit_value_t val;
			if (Reference* ref = dynamic_cast<Reference*>(ex)) {

				val = c.get_var(ref->variable->content).value;
				c.add_var(name, val, v->type, true);

			} else {
				val = ex->compile_jit(c, F, expressions.at(i)->type);
				c.set_var_type(name, ex->type);
				jit_insn_store(F, var, val);
				if (expressions.at(i)->type.must_manage_memory()) {
					VM::inc_refs(F, val);
				}
			}

			if (i == expressions.size() - 1) {
				if (ex->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, val, req_type);
				}
				return val;
			}
		} else {

			jit_value_t var = jit_value_create(F, JIT_INTEGER);
			c.add_var(name, var, Type::NULLL, false);

			jit_value_t val = JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
			jit_insn_store(F, var, val);
			VM::inc_refs(F, val);
		}
	}
	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}

}
