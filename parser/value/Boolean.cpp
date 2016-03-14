#include "../../vm/VM.hpp"
#include "Boolean.hpp"
#include "../../vm/value/LSBoolean.hpp"

using namespace std;

Boolean::Boolean(bool value) {
	this->value = value;
	type = Type::BOOLEAN;
	constant = true;
}

Boolean::~Boolean() {}

void Boolean::print(ostream& os) const {
	os << (value ? "true" : "false");
}

void Boolean::analyse(SemanticAnalyser* analyser, const Type) {

}

jit_value_t Boolean::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	if (req_type.nature == Nature::POINTER) {
		LSBoolean* b = new LSBoolean(value);
		return JIT_CREATE_CONST_POINTER(F, b);
	} else {
		return JIT_CREATE_CONST(F, JIT_INTEGER, value);
	}
}
