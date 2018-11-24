#include "Null_type.hpp"
#include "../colors.h"

namespace ls {

Null_type::Null_type() {}

Null_type::~Null_type() {}

bool Null_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Null_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "null" << END_COLOR;
	return os;
}

}