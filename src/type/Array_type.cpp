#include "Array_type.hpp"
#include "Ty.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

Type Array_type::element() const {
	return _element;
}
Type Set_type::element() const {
	return _element;
}
bool Array_type::operator == (const BaseRawType* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
bool Array_type::compatible(const BaseRawType* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element.compatible(array->_element);
	}
	return false;
}

bool Set_type::operator == (const BaseRawType* type) const {
	if (auto array = dynamic_cast<const Set_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
bool Set_type::compatible(const BaseRawType* type) const {
	if (auto set = dynamic_cast<const Set_type*>(type)) {
		return _element.compatible(set->_element);
	}
	return false;
}
// std::ostream& Array_type::print(std::ostream& os) const {
// 	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
// 	return os;
// }

}