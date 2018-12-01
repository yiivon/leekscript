#include "Null_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

bool Null_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Null_type*>(type);
}
llvm::Type* Null_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::string Null_type::clazz() const {
	return "Null";
}
std::ostream& Null_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "null" << END_COLOR;
	return os;
}

}