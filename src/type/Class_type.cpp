#include "Class_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Class_type::Class_type(std::string name) : Pointer_type(Type { std::make_shared<const Struct_type>(name, std::initializer_list<Type> {
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // refs
	Type::boolean() // native
}) }) {}

bool Class_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Class_type*>(type);
}
int Class_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const Class_type*>(type)) { return 0; }
	return -1;
}
std::string Class_type::clazz() const {
	return "Class";
}
std::ostream& Class_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "class" << END_COLOR;
	return os;
}

}