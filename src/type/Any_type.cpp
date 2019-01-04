#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"
#include "Struct_type.hpp"

namespace ls {

Any_type::Any_type() : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("any"), std::initializer_list<Type> {
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // refs
	Type::boolean() // native
}) }) {}

bool Any_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Any_type*>(type);
}
int Any_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 0; }
	return 100000;
}
std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}

}