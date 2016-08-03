#include "../../compiler/value/Boolean.hpp"

#include "../../vm/VM.hpp"
#include "../../vm/value/LSBoolean.hpp"

using namespace std;

namespace ls {

Boolean::Boolean(bool value) {
	this->value = value;
	type = Type::BOOLEAN;
	constant = true;
}

Boolean::~Boolean() {}

void Boolean::print(std::ostream& os, int, bool debug) const {
	os << (value ? "true" : "false");
	if (debug) {
		os << " " << type;
	}
}

unsigned Boolean::line() const {
	return 0;
}

void Boolean::analyse(SemanticAnalyser*, const Type& req_type) {
	if (req_type.nature == Nature::POINTER) {
		type = Type::BOOLEAN_P;
	}
}

jit_value_t Boolean::compile(Compiler& c) const {

	if (type.nature == Nature::POINTER) {
		LSBoolean* b = new LSBoolean(value);
		return JIT_CREATE_CONST_POINTER(c.F, b);
	} else {
		return JIT_CREATE_CONST(c.F, JIT_INTEGER, value);
	}
}

}
