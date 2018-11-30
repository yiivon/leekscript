#include "Class_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

llvm::Type* Class_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::ostream& Class_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "class" << END_COLOR;
	return os;
}

}