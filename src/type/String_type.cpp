#include "String_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

String_type::String_type() : Pointer_type(Type::structure("string", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // refs
	Type::boolean // native
})) {}

const Type* String_type::key() const {
	return Type::integer;
}
const Type* String_type::element() const {
	return Type::string;
}
const Type* String_type::iterator() const {
	return Type::structure("string_iterator", {
		Type::any, Type::integer, Type::integer, Type::integer, Type::integer
	});
}
bool String_type::operator == (const Type* type) const {
	return dynamic_cast<const String_type*>(type);
}
int String_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const String_type*>(type->folded)) { return 0; }
	return -1;
}
std::string String_type::class_name() const {
	return "String";
}
std::ostream& String_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "string" << END_COLOR;
	return os;
}
Type* String_type::clone() const {
	return new String_type {};
}

}