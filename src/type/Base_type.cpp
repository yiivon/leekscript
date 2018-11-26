#include "Base_type.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

Base_type::Base_type(int id, const std::string name) : _id(id), name(name) {}

Base_type::~Base_type() {}

bool Base_type::operator == (const Base_type& t) const {
	return _id == t._id;
}

bool Base_type::all(std::function<bool(const Base_type* const)> fun) const {
	return fun(this);
}

bool Base_type::compatible(std::shared_ptr<Base_type> type) const {}

int Base_type::id() const {
	return _id;
}

std::ostream& Base_type::print(std::ostream& os) const {
	os << BLUE_BOLD << name << END_COLOR;
	return os;
}

std::ostream& operator << (std::ostream& os, const Base_type* type) {
	type->print(os);
	return os;
}

}