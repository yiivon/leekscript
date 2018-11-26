#include "Int_type.hpp"
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

int Int_type::_id = Ty::get_next_id();

Int_type::Int_type() : Long_type(_id, "int") {}
Int_type::~Int_type() {}

std::ostream& Int_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "int" << END_COLOR;
	return os;
}

}