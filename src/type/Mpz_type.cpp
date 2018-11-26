#include "Mpz_type.hpp"
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

int Mpz_type::_id = Ty::get_next_id();

Mpz_type::Mpz_type() : Integer_type(_id, "mpz") {}
Mpz_type::~Mpz_type() {}

std::ostream& Mpz_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "mpz" << END_COLOR;
	return os;
}

}