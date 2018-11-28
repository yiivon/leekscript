#include "Number_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

int Number_type::_id = Ty::get_next_id();

Number_type::Number_type(int id, const std::string name) : Base_type(_id * id, name) {}
Number_type::~Number_type() {}

std::ostream& Number_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "number" << END_COLOR;
	return os;
}

}