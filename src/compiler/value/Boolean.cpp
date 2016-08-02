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

void Boolean::print(std::ostream& os) const {
	os << (value ? "true" : "false");
}

unsigned Boolean::line() const {
	return 0;
}

void Boolean::analyse(SemanticAnalyser*, const Type) {}

jit_value_t Boolean::compile_jit(Compiler&, jit_function_t& F, Type req_type) const {

	if (req_type.nature == Nature::POINTER) {
		LSBoolean* b = new LSBoolean(value);
		return JIT_CREATE_CONST_POINTER(F, b);
	} else {
		return JIT_CREATE_CONST(F, JIT_INTEGER, value);
	}
}

}
