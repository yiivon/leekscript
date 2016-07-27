#include "../../compiler/value/VariableValue.hpp"

#include "../../vm/VM.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

VariableValue::VariableValue(Token* token) {
	this->name = token->content;
	this->token = token;
	this->var = nullptr;
	constant = false;
}

VariableValue::~VariableValue() {}

void VariableValue::print(ostream& os) const {
	os << token->content;
}

int VariableValue::line() const {
	return token->line;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type) {

	var = analyser->get_var(token);
	if (var != nullptr) {
		type = var->type;
		attr_types = var->attr_types;
	}
//	cout << "VV " << name->content << " : " << type << endl;
//	cout << "var scope : " << (int)var->scope << endl;
//	for (auto t : attr_types)
//		cout << t.first << " : " << t.second << endl;
}

void VariableValue::must_return(SemanticAnalyser* analyser, const Type& ret_type) {

	var->value->must_return(analyser, ret_type);

	type.setReturnType(ret_type);
}

/*
 * let a = 12 // integer
 * a = 5.6 // float
 * a = 12 // integer
 * a += 1.2 // float at the previous assignment
 */
void VariableValue::change_type(SemanticAnalyser* analyser, const Type& type) {
	if (var != nullptr) {
		var->type = type;
		this->type = type;
	}
}

extern map<string, jit_value_t> internals;

jit_value_t VariableValue::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

//	cout << "compile vv " << name->content << " : " << type << endl;
//	cout << "req type : " << req_type << endl;

	jit_value_t v;

	if (var->scope == VarScope::INTERNAL) {

		v = internals[name];

	} else if (var->scope == VarScope::LOCAL) {

		v = c.get_var(name).value;

	} else { // Parameter

		v = jit_value_get_param(F, var->index);
	}

	if (var->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
		return VM::value_to_pointer(F, v, var->type);
	}
	return v;
}

jit_value_t VariableValue::compile_jit_l(Compiler& c, jit_function_t& F, Type) const {

	return compile_jit(c, F, type);
}

}
