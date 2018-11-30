#include "Object_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

bool Object_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Object_type*>(type);
}
llvm::Type* Object_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& Object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "object" << END_COLOR;
	return os;
}

}