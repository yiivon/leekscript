#include "Function_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

bool Function_type::operator == (const Base_type* type) const {
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		return true;
	}
	return false;
}
llvm::Type* Function_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::string Function_type::clazz() const {
	return "Function";
}
std::ostream& Function_type::print(std::ostream& os) const {
	os << BLUE_BOLD << (_closure ? "closure" : "function") << END_COLOR;
	return os;
}

}