#include "../../compiler/value/Number.hpp"

#include "../../vm/VM.hpp"
#include <limits.h>
#include "../../vm/value/LSNumber.hpp"

using namespace std;

namespace ls {

Number::Number(std::string value, Token* token) {
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

	double double_value;
	bool large_number = false;
	try {
		double_value = stod(value);
	} catch (exception& e) {
		large_number = true;
	}
	large_number |= token->large_number;

	if (large_number) {
		type = Type::GMP_INT;
	} else {
		int int_value = (int) double_value;
		if (int_value != double_value or req_type.raw_type == RawType::REAL) {
			type = Type::REAL;
		} else {
			type = Type::INTEGER;
		}
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

jit_value_t Number::compile(Compiler& c) const {

	if (type.nature == Nature::POINTER) {

		jit_value_t val = LS_CREATE_REAL(c.F, stod(value));
		return VM::value_to_pointer(c.F, val, Type::REAL);

	} else if (type.raw_type == RawType::REAL) {

		return LS_CREATE_REAL(c.F, stod(value));

	} else if (type.raw_type == RawType::GMP_INT) {

		mpz_t mpz;
		mpz_init_set_str(mpz, value.c_str(), 10);

		jit_value_t gmp_struct = jit_value_create(c.F, VM::gmp_int_type);
		jit_value_set_addressable(gmp_struct);
		jit_insn_store_relative(c.F, jit_insn_address_of(c.F, gmp_struct), 0, LS_CREATE_INTEGER(c.F, mpz->_mp_alloc));
		jit_insn_store_relative(c.F, jit_insn_address_of(c.F, gmp_struct), 4, LS_CREATE_INTEGER(c.F, mpz->_mp_size));
		jit_insn_store_relative(c.F, jit_insn_address_of(c.F, gmp_struct), 8, LS_CREATE_POINTER(c.F, mpz->_mp_d));

		VM::gmp_values_created++;
		return gmp_struct;

	} else {
		return LS_CREATE_INTEGER(c.F, stoi(value));
	}
}

}
