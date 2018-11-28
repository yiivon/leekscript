#include "Long_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

int Long_type::ID = Ty::get_next_id();

Long_type::Long_type(int id, const std::string name) : Integer_type(ID * id, name) {}
Long_type::~Long_type() {}

std::ostream& Long_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "long" << END_COLOR;
	return os;
}

}