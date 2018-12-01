#include "Base_type.hpp"
#include "Type.hpp"
#include "../vm/LSValue.hpp"

namespace ls {

Base_type::~Base_type() {}

Type Base_type::element() const {
	return {};
}
Type Base_type::key() const {
	return {};
}
bool Base_type::operator == (const Base_type*) const {
	return false;
}
bool Base_type::compatible(const Base_type*) const {
	return false;
}
Type Base_type::iterator() const {
	assert(false && "No iterator available on this type");
}
std::string Base_type::clazz() const {
	return "";
}
std::ostream& Base_type::print(std::ostream& os) const {
	os << "???";
	return os;
}

}