#include "Double_type.hpp"
#include "../colors.h"

namespace ls {

Double_type::Double_type() {}

Double_type::~Double_type() {}

bool Double_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Double_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "double" << END_COLOR;
	return os;
}

}