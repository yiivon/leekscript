#include "Set_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

Type Set_type::element() const {
	return _element;
}
bool Set_type::operator == (const Base_type* type) const {
	if (auto array = dynamic_cast<const Set_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
bool Set_type::compatible(const Base_type* type) const {
	if (auto set = dynamic_cast<const Set_type*>(type)) {
		return _element.compatible(set->_element);
	}
	return false;
}
llvm::Type* Set_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::string Set_type::clazz() const {
	return "Set";
}
std::ostream& Set_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
	return os;
}

}