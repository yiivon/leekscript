#include "../../compiler/value/Reference.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

Reference::Reference() {
	type = Type::VALUE;
	variable = nullptr;
	var = nullptr;
}

Reference::~Reference() {}

void Reference::print(ostream& os) const {
	os << "@" << variable;
}

void Reference::analyse(SemanticAnalyser* analyser, const Type) {

	var = analyser->get_var(variable);
	type = var->type;

//	cout << "ref " << variable->content << " : " << type << endl;
}

extern map<string, jit_value_t> internals;
extern map<string, jit_value_t> globals;
extern map<string, jit_value_t> locals;

jit_value_t Reference::compile_jit(Compiler&, jit_function_t& F, Type req_type) const {

//	cout << "Reference compile()" << endl;

	/*
	jit_value_t v;

	if (var->scope == VarScope::INTERNAL) {
		v = internals[variable->content];
	} else if (var->scope == VarScope::GLOBAL) {
		v = globals[variable->content];
	} else if (var->scope == VarScope::LOCAL) {
		v = locals[variable->content];
	} else { // var->scope == VarScope::PARAMETER
		v = jit_value_get_param(F, var->index);
	}

	if (var->type.nature == Nature::POINTER) {
		VM::inc_refs(F, v);
		return v;
	}
	if (req_type.nature == Nature::POINTER) {
		jit_value_t val = VM::value_to_pointer(F, v, req_type);
		VM::inc_refs(F, val);
		return val;
	}
	return v;
	*/

	if (req_type.nature == Nature::POINTER) {
		LSValue* n = LSNull::null_var;
		return JIT_CREATE_CONST_POINTER(F, n);
	} else {
		return JIT_CREATE_CONST(F, JIT_INTEGER, 0);
	}
}

}
