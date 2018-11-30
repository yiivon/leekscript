#include "Function_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

llvm::Type* FunctionRawType::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}

Function_type::Function_type() : Base_type(1, "function") {}
Function_type::~Function_type() {}

bool Function_type::compatible(std::shared_ptr<Base_type>) const {
	return true;
}

std::ostream& Function_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "function" << END_COLOR;
	return os;
}

}