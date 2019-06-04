#include "Base_type.hpp"
#include "Type.hpp"
#include "../vm/LSValue.hpp"
#include "Any_type.hpp"

namespace ls {

Base_type::~Base_type() {}

const Type* Base_type::element() const {
	return Type::any;
}
const Type* Base_type::key() const {
	return Type::any;
}
const Type* Base_type::member(int) const {
	return Type::any;
}
bool Base_type::operator == (const Base_type*) const {
	return false;
}
bool Base_type::castable(const Base_type*) const {
	return false;
}
bool Base_type::castable(const Type& type) const {
	return type.all([&](const Base_type* t) {
		return castable(t);
	});
}
int Base_type::distance(const Base_type* type) const {
	return -1;
}
const Type* Base_type::iterator() const {
	assert(false && "No iterator available on this type");
}
const Type* Base_type::return_type() const {
	// assert(false && "No return type on this type");
	return Type::any;
}
const Type* Base_type::argument(size_t) const {
	// assert(false && "No arguments on this type");
	return Type::any;
}
const std::vector<const Type*>& Base_type::arguments() const {
	// assert(false && "No arguments on this type");
	return Type::empty_types;
}
std::string Base_type::clazz() const {
	return "";
}
std::ostream& Base_type::print(std::ostream& os) const {
	os << "???";
	return os;
}
std::ostream& operator << (std::ostream& os, const Base_type* type) {
	return type->print(os);
}

}