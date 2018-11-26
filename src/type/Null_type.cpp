#include "Null_type.hpp"
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

int Null_type::_id = Ty::get_next_id();

Null_type::Null_type() : Base_type(_id, "null") {}
Null_type::~Null_type() {}

std::ostream& Null_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "null" << END_COLOR;
	return os;
}

}