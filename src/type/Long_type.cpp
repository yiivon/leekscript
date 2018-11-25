#include "Long_type.hpp"
#include "../colors.h"

namespace ls {

Long_type::Long_type() {}
Long_type::~Long_type() {}

bool Long_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Long_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "integer" << END_COLOR;
	return os;
}

}