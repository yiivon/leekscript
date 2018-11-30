#include "Number_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

llvm::Type* Number_type::llvm() const {
	assert(false);
}
std::ostream& Number_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "number" << END_COLOR;
	return os;
}

}