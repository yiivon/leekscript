#include "List_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

List_type::~List_type() {}

std::shared_ptr<Base_type> List_type::element() const {
	return element_type;
}

bool List_type::compatible(std::shared_ptr<Base_type> type) const {
	return type->all([&](const auto& type) {
		if (auto list = dynamic_cast<const List_type*>(type)) {
			return element_type->compatible(list->element());
		}
		return false;
	});
}

std::ostream& List_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "list" << END_COLOR << "<" << element_type << ">";
	return os;
}

}