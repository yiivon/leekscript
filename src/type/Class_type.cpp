#include "Class_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Class_type::Class_type(std::string name) : Pointer_type(Type::structure(name, {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // refs
	Type::boolean // native
}), true), name(name) {}

bool Class_type::operator == (const Type* type) const {
	return dynamic_cast<const Class_type*>(type);
}
int Class_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Class_type*>(type->folded)) { return 0; }
	return -1;
}
std::string Class_type::class_name() const {
	return "Class";
}
std::ostream& Class_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "class" << END_COLOR;
	return os;
}
Type* Class_type::clone() const {
	return new Class_type { name };
}

}