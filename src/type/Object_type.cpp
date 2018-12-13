#include "Object_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"

namespace ls {

bool Object_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Object_type*>(type);
}
int Object_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const Object_type*>(type)) { return 0; }
	return -1;
}
llvm::Type* Object_type::llvm() const {
	return Any_type::get_any_type();
}
std::string Object_type::clazz() const {
	return "Object";
}
std::ostream& Object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "object" << END_COLOR;
	return os;
}

}