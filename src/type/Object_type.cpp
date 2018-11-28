#include "Object_type.hpp"
#include "../colors.h"

namespace ls {

Object_type::Object_type() : Base_type(1, "object") {}
Object_type::~Object_type() {}

bool Object_type::compatible(std::shared_ptr<Base_type>) const {
	return true;
}

std::ostream& Object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "object" << END_COLOR;
	return os;
}

}