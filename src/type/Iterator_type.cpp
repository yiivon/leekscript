#include "Iterator_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

bool Iterator_type::operator == (const BaseRawType* type) const {
	if (auto array = dynamic_cast<const Iterator_type*>(type)) {
		return _container == array->_container;
	}
	return false;
}
bool Iterator_type::compatible(const BaseRawType* type) const {
	if (auto array = dynamic_cast<const Iterator_type*>(type)) {
		return _container.compatible(array->_container);
	}
	return false;
}

// std::ostream& Array_type::print(std::ostream& os) const {
// 	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
// 	return os;
// }

}