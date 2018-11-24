#include "Bool_type.hpp"
#include "../colors.h"

namespace ls {

Bool_type::Bool_type() {}

Bool_type::~Bool_type() {}

bool Bool_type::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

std::ostream& Bool_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "bool" << END_COLOR;
	return os;
}

}