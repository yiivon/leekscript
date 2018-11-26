#include "List_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

int List_type::_id = Ty::get_next_id();

List_type::List_type(std::shared_ptr<Ty> element, int id, const std::string name) : Base_type(_id * id, name), element_type(element) {}
List_type::~List_type() {}

std::shared_ptr<Ty> List_type::element() const {
	return element_type;
}



std::ostream& List_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "list" << END_COLOR << "<" << *element_type << ">";
	return os;
}

}