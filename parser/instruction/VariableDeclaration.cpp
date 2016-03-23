#include "VariableDeclaration.hpp"
#include "../../vm/VM.hpp"
#include "../semantic/SemanticAnalyser.hpp"

using namespace std;

VariableDeclaration::VariableDeclaration() {
	global = false;
	return_value = false;
	type = Type::VALUE;
}

VariableDeclaration::~VariableDeclaration() {}

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

		// Global Variable already defined ? We don't define another time
		if (vars.find(var->content) == vars.end() || vars.at(var->content)->scope != VarScope::GLOBAL) {
			SemanticVar* v = analyser->add_var(var, type, value);
			vars.insert(pair<string, SemanticVar*>(var->content, v));
		}
		vars.at(var->content)->type = type;
	}
	this->return_value = return_value;
}

extern map<string, jit_value_t> globals;
extern map<string, Type> globals_types;
extern map<string, jit_value_t> locals;

jit_value_t VariableDeclaration::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	for (unsigned i = 0; i < variables.size(); ++i) {

		SemanticVar* v = vars.at(variables.at(i)->content);

		if (v->scope == VarScope::GLOBAL) {

//			cout << "add global var : " << variables[i] << endl;

			jit_value_t var = jit_value_create(F, JIT_INTEGER_LONG);
			globals.insert(pair<string, jit_value_t>(variables[i]->content, var));
			if (i < expressions.size()) {

				jit_value_t val = expressions.at(i)->compile_jit(c, F, Type::NEUTRAL);
				globals_types.insert(pair<string, Type>(variables[i]->content, expressions[i]->type));
				jit_insn_store(F, var, val);

				if (i == expressions.size() - 1) {
					if (expressions[i]->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
						return VM::value_to_pointer(F, val, req_type);
					}
					return val;
				}
			} else {

				globals_types.insert(pair<string, Type>(variables[i]->content, Type::NULLL));
				jit_value_t val = JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
				jit_insn_store(F, var, val);
			}
		} else {

			jit_value_t var = jit_value_create(F, JIT_INTEGER);
			locals.insert(pair<string, jit_value_t>(variables[i]->content, var));

			if (i < expressions.size()) {

				jit_value_t val = expressions.at(i)->compile_jit(c, F, Type::NEUTRAL);

				jit_insn_store(F, var, val);

				if (i == variables.size() - 1) {
					if (expressions[i]->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
						return VM::value_to_pointer(F, val, req_type);
					}
					return val;
				}
			} else {

				jit_value_t val = JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
				jit_insn_store(F, var, val);
				return val;
			}
		}
	}
	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}
