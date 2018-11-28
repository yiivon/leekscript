#include "Class_type.hpp"
#include "../colors.h"

namespace ls {

Class_type::Class_type() : Base_type(1, "class") {}
Class_type::~Class_type() {}

bool Class_type::compatible(std::shared_ptr<Base_type>) const {
	return true;
}

std::ostream& Class_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "class" << END_COLOR;
	return os;
}

}