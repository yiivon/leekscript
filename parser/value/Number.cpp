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

void Number::analyse(SemanticAnalyser* analyser, const Type req_type) {

	if (value != (int) value or req_type.raw_type == RawType::FLOAT) {
		type = Type::FLOAT;
	} else {
		type = Type::INTEGER;
	}

	if (req_type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
}

jit_value_t Number::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	if (req_type.nature == Nature::POINTER) {

		LSNumber* n = new LSNumber(value);
		return JIT_CREATE_CONST_POINTER(F, n);

	} else {
		bool isfloat = type.raw_type == RawType::FLOAT;
		jit_type_t type = isfloat ? JIT_FLOAT : JIT_INTEGER;

		jit_type_t tagged_type = jit_type_create_tagged(type, 12, nullptr, nullptr, 0);

		if (isfloat) {
			return JIT_CREATE_CONST_FLOAT(F, tagged_type, value);
		} else {
			return JIT_CREATE_CONST(F, tagged_type, value);
		}
	}
}
