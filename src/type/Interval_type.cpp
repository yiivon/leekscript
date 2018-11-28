#include "Interval_type.hpp"
#include "../colors.h"

namespace ls {

int Interval_type::_id = Ty::get_next_id();

Interval_type::Interval_type(int id, const std::string name) : List_type(Ty::get_int(), _id * id, "interval") {}
Interval_type::~Interval_type() {}

bool Interval_type::compatible(std::shared_ptr<Base_type>) const {
	return true;
}

std::ostream& Interval_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "interval" << END_COLOR;
	return os;
}

}