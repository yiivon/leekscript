#include "../../compiler/value/Number.hpp"

#include "../../vm/VM.hpp"
#include <limits.h>
#include "../../vm/value/LSNumber.hpp"

using namespace std;

namespace ls {

Number::Number(double value, Token* token) {
	this->value = value;
	this->token = token;
	constant = true;
}

Number::~Number() {
}

void Number::print(ostream& os) const {
	os << value;
}

unsigned Number::line() const {
	return token->line;
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

		jit_value_t val = JIT_CREATE_CONST_FLOAT(F, ls_jit_real, value);
		return VM::value_to_pointer(F, val, Type::FLOAT);

	} else {

		bool isfloat = type.raw_type == RawType::FLOAT;
		jit_type_t type = isfloat ? ls_jit_real : ls_jit_integer;

		if (isfloat) {
			return JIT_CREATE_CONST_FLOAT(F, type, value);
		} else {
			return JIT_CREATE_CONST(F, type, value);
		}
	}
}

}
