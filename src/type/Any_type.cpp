#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

llvm::Type* AnyRawType::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}

Any_type::Any_type() : Base_type(1, "any") {}
Any_type::~Any_type() {}

bool Any_type::compatible(std::shared_ptr<Base_type>) const {
	return true;
}

std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}

}