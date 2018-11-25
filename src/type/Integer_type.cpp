#include "Integer_type.hpp"
#include "../colors.h"

namespace ls {

Integer_type::Integer_type() {}

Integer_type::~Integer_type() {}

bool Integer_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Integer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "integer" << END_COLOR;
	return os;
}

}