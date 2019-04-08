#include "Never_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

Never_type::Never_type() {}

bool Never_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Never_type*>(type);
}
int Never_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Never_type*>(type)) { return 0; }
	return 100000 + type->distance(this);
}
std::ostream& Never_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "never" << END_COLOR;
	return os;
}

}