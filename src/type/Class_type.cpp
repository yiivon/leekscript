#include "Class_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

bool Class_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Class_type*>(type);
}
llvm::Type* Class_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& Class_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "class" << END_COLOR;
	return os;
}

}