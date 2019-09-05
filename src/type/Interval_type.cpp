#include "Interval_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Interval_type::Interval_type() : Pointer_type(Type::structure("interval", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::boolean, // native
	Type::integer, // A
	Type::integer // B
})) {}

const Type* Interval_type::key() const {
	return Type::integer;
}
const Type* Interval_type::element() const {
	return Type::integer;
}
const Type* Interval_type::iterator() const {
	return Type::structure("interval_iterator", {
		Type::interval, Type::integer
	});
}
bool Interval_type::operator == (const Type* type) const {
	return dynamic_cast<const Interval_type*>(type);
}
int Interval_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Interval_type*>(type->folded)) { return 0; }
	return -1;
}
std::string Interval_type::class_name() const {
	return "Interval";
}
std::ostream& Interval_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "interval" << END_COLOR;
	return os;
}
Type* Interval_type::clone() const {
	return new Interval_type {};
}

}