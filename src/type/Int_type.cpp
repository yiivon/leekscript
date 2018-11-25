#include "Int_type.hpp"
#include "../colors.h"

namespace ls {

Int_type::Int_type() {}
Int_type::~Int_type() {}

bool Int_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Int_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "int" << END_COLOR;
	return os;
}

}