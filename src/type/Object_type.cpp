#include "Object_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"

namespace ls {

Object_type::Object_type() : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("object"), std::initializer_list<Type> {
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // refs
	Type::boolean() // native
}) }) {}

bool Object_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Object_type*>(type);
}
int Object_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const Object_type*>(type)) { return 0; }
	return -1;
}
std::string Object_type::clazz() const {
	return "Object";
}
std::ostream& Object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "object" << END_COLOR;
	return os;
}

}