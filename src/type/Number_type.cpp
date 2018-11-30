#include "Number_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

bool Number_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Number_type*>(type);
}
llvm::Type* Number_type::llvm() const {
	assert(false);
}
std::ostream& Number_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "number" << END_COLOR;
	return os;
}

}