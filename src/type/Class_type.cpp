#include "Class_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"

namespace ls {

bool Class_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Class_type*>(type);
}
llvm::Type* Class_type::llvm() const {
	return Any_type::get_any_type();
}
std::string Class_type::clazz() const {
	return "Class";
}
std::ostream& Class_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "class" << END_COLOR;
	return os;
}

}