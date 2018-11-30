#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

bool Any_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Any_type*>(type);
}
llvm::Type* Any_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}

bool Placeholder_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Placeholder_type*>(type);
}

}