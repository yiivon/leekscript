#include "Any_type.hpp"
#include "../colors.h"

namespace ls {

Any_type::~Any_type() {}

bool Any_type::compatible(std::shared_ptr<Base_type>) const {
	return true;
}

std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}

}