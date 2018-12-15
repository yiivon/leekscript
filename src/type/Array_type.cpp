#include "Array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "../compiler/LLVMCompiler.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"

namespace ls {

Array_type::Array_type(Type element) : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("_array"), std::initializer_list<Type> {
	Type::INTEGER, // ?
	Type::INTEGER, // ?
	Type::INTEGER, // ?
	Type::INTEGER, // ?
	Type::BOOLEAN, // native
	element.pointer(), // vector.begin
	element.pointer(), // vector.end
	element.pointer() // vector.data
}) }), _element(element) {
}

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
		return _element._types.size() == 0 or _element.fold() == array->_element.fold();
	}
	return false;
}
int Array_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1000; }
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		if (array->_element._types.size() == 0) {
			return 999;
		}
		return _element.distance(array->_element);
	}
	return -1;
}
Type Array_type::iterator() const {
	const auto merged = _element.fold();
	if (merged.is_integer()) return Type::INT_ARRAY_ITERATOR;
	if (merged.is_real()) return Type::REAL_ARRAY_ITERATOR;
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