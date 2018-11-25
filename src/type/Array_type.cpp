#include "Array_type.hpp"
#include "LType.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

Array_type::Array_type() : element_type(std::make_shared<LType>(new Any_type())) {}

Array_type::Array_type(std::shared_ptr<Base_type> element) : element_type(element) {}

Array_type::~Array_type() {}

std::shared_ptr<Base_type> Array_type::element() const {
	return element_type;
}

bool Array_type::compatible(std::shared_ptr<Base_type> type) const {
	return type->all([&](const auto& type) {
		if (auto array = dynamic_cast<const Array_type*>(type)) {
			return element_type->compatible(array->element());
		}
		return false;
	});
}

std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << element_type << ">";
	return os;
}

}