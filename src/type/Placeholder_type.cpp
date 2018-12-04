#include "Placeholder_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

bool Placeholder_type::operator == (const Base_type* type) const {
	return this == type;
}
llvm::Type* Placeholder_type::llvm() const {
	assert("Placeholder type should not be compiled");
}
std::ostream& Placeholder_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name << END_COLOR;
	return os;
}

}