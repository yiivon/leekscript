#include "Mpz_type.hpp"
#include "../colors.h"

namespace ls {

Mpz_type::Mpz_type() {}
Mpz_type::~Mpz_type() {}

bool Mpz_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Mpz_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "integer" << END_COLOR;
	return os;
}

}