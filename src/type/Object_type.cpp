#include "Object_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

llvm::Type* Object_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& Object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "object" << END_COLOR;
	return os;
}

}