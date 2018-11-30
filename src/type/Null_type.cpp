#include "Null_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

llvm::Type* Null_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& Null_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "null" << END_COLOR;
	return os;
}

}