#include "Null_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "Pointer_type.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"

namespace ls {

Null_type::Null_type() : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("null"), std::initializer_list<Type> {
	Type::INTEGER, // ?
	Type::INTEGER, // ?
	Type::INTEGER, // ?
	Type::INTEGER, // refs
	Type::BOOLEAN // native
}) }) {}

bool Null_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Null_type*>(type);
}
int Null_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const Null_type*>(type)) { return 0; }
	return -1;
}
std::string Null_type::clazz() const {
	return "Null";
}
std::ostream& Null_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "null" << END_COLOR;
	return os;
}

}