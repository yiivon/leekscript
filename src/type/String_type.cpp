#include "String_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"
#include "Any_type.hpp"

namespace ls {

bool String_type::operator == (const Base_type* type) const {
	return dynamic_cast<const String_type*>(type);
}
int String_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const String_type*>(type)) { return 0; }
	return -1;
}
llvm::Type* String_type::llvm() const {
	return Any_type::get_any_type();
}
std::string String_type::clazz() const {
	return "String";
}
std::ostream& String_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "string" << END_COLOR;
	return os;
}

}