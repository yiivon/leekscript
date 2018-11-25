#include "String_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

String_type::String_type() {}
String_type::~String_type() {}

bool String_type::compatible(std::shared_ptr<Base_type> type) const {
	return false;
}

std::ostream& String_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "string" << END_COLOR;
	return os;
}

}