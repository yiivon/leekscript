#include "Interval_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Interval_type::Interval_type() : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("_interval"), std::initializer_list<Type> {
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::boolean(), // native
	Type::integer(), // A
	Type::integer() // B
}) }) {}

Type Interval_type::key() const {
	return Type::integer();
}
Type Interval_type::element() const {
	return Type::integer();
}
Type Interval_type::iterator() const {
	return Type::structure("interval_iterator", {
		Type::interval(), Type::integer()
	});
}
bool Interval_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Interval_type*>(type);
}
int Interval_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const Interval_type*>(type)) { return 0; }
	return -1;
}
std::string Interval_type::clazz() const {
	return "Interval";
}
std::ostream& Interval_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "interval" << END_COLOR;
	return os;
}

}