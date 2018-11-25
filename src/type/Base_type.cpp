#include "Base_type.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>

namespace ls {

Base_type::~Base_type() {}

bool Base_type::operator == (const Base_type& t) const {
	return typeid(*this) == typeid(t);
}

bool Base_type::all(std::function<bool(const Base_type* const)> fun) const {
	return fun(this);
}

std::ostream& operator << (std::ostream& os, const Base_type* type) {
	type->print(os);
	return os;
}

}