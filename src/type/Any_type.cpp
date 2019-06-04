#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"
#include "Struct_type.hpp"

namespace ls {

Any_type::Any_type() : Pointer_type(Type::structure("any", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // refs
	Type::boolean // native
})) {}

bool Any_type::operator == (const Type* type) const {
	return dynamic_cast<const Any_type*>(type);
}
int Any_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 0; }
	return 100000 + type->distance(this);
}
std::string Any_type::class_name() const {
	return "Value";
}
std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}
Type* Any_type::clone() const {
	return new Any_type {};
}

}