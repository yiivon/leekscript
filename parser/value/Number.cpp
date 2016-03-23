#include "../../vm/VM.hpp"
#include "Number.hpp"
#include <limits.h>

using namespace std;

Number::Number(double value) {
	this->value = value;
	constant = true;
}

Number::~Number() {}

void Number::print(ostream& os) const {
	os << value;
}

void Number::analyse(SemanticAnalyser*, const Type req_type) {

	if (value != (int) value or req_type.raw_type == RawType::FLOAT) {
		type = Type::FLOAT;
	} else {
		type = Type::INTEGER;
	}

	if (req_type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
}

jit_value_t Number::compile_jit(Compiler&, jit_function_t& F, Type req_type) const {

	if (req_type.nature == Nature::POINTER) {

		LSNumber* n = LSNumber::get(value);
		return JIT_CREATE_CONST_POINTER(F, n);

	} else {
		bool isfloat = type.raw_type == RawType::FLOAT;
		jit_type_t type = isfloat ? JIT_FLOAT : JIT_INTEGER;

		if (isfloat) {
			return JIT_CREATE_CONST_FLOAT(F, type, value);
		} else {
			return JIT_CREATE_CONST(F, type, value);
		}
	}
}
