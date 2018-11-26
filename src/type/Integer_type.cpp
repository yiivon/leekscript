#include "Integer_type.hpp"
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

int Integer_type::_id = Ty::get_next_id();

Integer_type::Integer_type(int id, const std::string name) : Number_type(_id * id, name) {}
Integer_type::~Integer_type() {}

std::ostream& Integer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "integer" << END_COLOR;
	return os;
}

}