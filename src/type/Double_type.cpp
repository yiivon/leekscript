#include "Double_type.hpp"
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

int Double_type::_id = Ty::get_next_id();

Double_type::Double_type() : Real_type(_id, "double") {}
Double_type::~Double_type() {}

std::ostream& Double_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "double" << END_COLOR;
	return os;
}

}