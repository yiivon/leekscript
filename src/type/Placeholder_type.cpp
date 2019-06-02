#include "Placeholder_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"

namespace ls {

void Placeholder_type::implement(const Type* implementation) const {
	((Placeholder_type*) this)->_implementation = implementation;
}
bool Placeholder_type::operator == (const Base_type* type) const {
	return this == type;
}
bool Placeholder_type::compatible(const Base_type* type) const {
	return false;
}
int Placeholder_type::distance(const Base_type* type) const {
	return 0;
}
std::ostream& Placeholder_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name;
	if (_implementation != nullptr) {
		os << "." << _implementation;
	}
	os << END_COLOR;
	return os;
}

}