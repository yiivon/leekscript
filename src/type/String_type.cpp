#include "String_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

String_type::String_type() : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("string"), std::initializer_list<Type> {
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // refs
	Type::boolean() // native
}) }) {}

const Type& String_type::key() const {
	return Type::integer_type;
}
const Type& String_type::element() const {
	return Type::string_type;
}
Type String_type::iterator() const {
	return Type::structure("string_iterator", {
		Type::any(), Type::integer(), Type::integer(), Type::integer(), Type::integer()
	});
}
bool String_type::operator == (const Base_type* type) const {
	return dynamic_cast<const String_type*>(type);
}
int String_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const String_type*>(type)) { return 0; }
	return -1;
}
std::string String_type::clazz() const {
	return "String";
}
std::ostream& String_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "string" << END_COLOR;
	return os;
}

}