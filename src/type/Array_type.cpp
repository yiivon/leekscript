#include "Array_type.hpp"
#include "Ty.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

int Array_type::_id = Ty::get_next_id();

Array_type::Array_type(int id) : List_type(Ty::any(), _id * id, "array") {}

Array_type::Array_type(Ty element, int id) : List_type(element, _id * id, "array") {}

Array_type::~Array_type() {}

bool Array_type::compatible(std::shared_ptr<Base_type> type) const {
	// return type->all([&](const auto& type) {
	// 	if (auto array = dynamic_cast<const Array_type*>(type)) {
	// 		return element_type->compatible(array->element());
	// 	}
	// 	return false;
	// });
	return false;
}

std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << element_type << ">";
	return os;
}

}