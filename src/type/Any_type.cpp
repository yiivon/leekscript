#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

llvm::Type* AnyRawType::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& AnyRawType::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}

}