#include "Real_type.hpp"
#include "../colors.h"

namespace ls {

Real_type::Real_type() {}

Real_type::~Real_type() {}

bool Real_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Real_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "real" << END_COLOR;
	return os;
}

}