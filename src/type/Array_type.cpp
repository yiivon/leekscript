#include "Array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>

namespace ls {

Type Array_type::element() const {
	return _element;
}
bool Array_type::operator == (const Base_type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
bool Array_type::compatible(const Base_type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element.compatible(array->_element);
	}
	return false;
}
llvm::Type* Array_type::llvm() const {
	if (_element == Type::INTEGER) {
		return Type::LLVM_VECTOR_INT_TYPE_PTR;
	} else if (_element == Type::REAL) {
		return Type::LLVM_VECTOR_REAL_TYPE_PTR;
	} else {
		return Type::LLVM_VECTOR_TYPE_PTR;
	}
}
Type Array_type::iterator() const {
	if (_element == Type::INTEGER) return Type::INT_ARRAY_ITERATOR;
	if (_element == Type::REAL) return Type::REAL_ARRAY_ITERATOR;
	return Type::PTR_ARRAY_ITERATOR;
}
std::string Array_type::clazz() const {
	return "Array";
}
std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
	return os;
}

}