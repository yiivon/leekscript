#include "String_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"

namespace ls {

llvm::Type* String_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& String_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "string" << END_COLOR;
	return os;
}

}