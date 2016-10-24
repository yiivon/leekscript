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

void Number::print(ostream& os, int, bool debug) const {
	os << value;
	if (debug) {
		os << " " << type;
	}
}

unsigned Number::line() const {
	return token->line;
}

void Number::analyse(SemanticAnalyser*, const Type& req_type) {

	if (value != (int) value or req_type.raw_type == RawType::REAL) {
		type = Type::REAL;
	} else {
		type = Type::INTEGER;
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

jit_value_t Number::compile(Compiler& c) const {

	if (type.nature == Nature::POINTER) {

		jit_value_t val = LS_CREATE_REAL(c.F, value);
		return VM::value_to_pointer(c.F, val, Type::REAL);

	} else {

		if (type.raw_type == RawType::REAL) {
			return LS_CREATE_REAL(c.F, value);
		} else {
			return LS_CREATE_INTEGER(c.F, value);
		}
	}
}

}
