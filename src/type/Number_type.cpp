#include "Number_type.hpp"
#include "../colors.h"

namespace ls {

Number_type::Number_type() {}

Number_type::~Number_type() {}

bool Number_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Number_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "number" << END_COLOR;
	return os;
}

}