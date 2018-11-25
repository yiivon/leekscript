#include "LType.hpp"
#include <algorithm>
#include "../colors.h"
#include <iostream>

namespace ls {

LType::LType() {}

LType::LType(Base_type* type) {
	_types.push_back(std::shared_ptr<Base_type>(type));
}

std::vector<std::shared_ptr<Base_type>> LType::types() const {
	return _types;
}

void LType::add(const std::shared_ptr<Base_type> type) {
	_types.push_back(type);
}

bool LType::operator == (const LType& type) const {
	return _types.size() == type._types.size() && std::equal(_types.begin(), _types.end(), type._types.begin(), [](std::shared_ptr<Base_type> a, std::shared_ptr<Base_type> b) {
		return *a == *b;
	});
}

bool LType::compatible(std::shared_ptr<Base_type>) const {
	return false;
}

bool LType::all(std::function<bool(const Base_type* const)> fun) const {
	return std::all_of(begin(_types), end(_types), [&](auto& type) {
		return fun(type.get());
	});
}

bool LType::iterable() const {
	return std::all_of(begin(_types), end(_types), [](auto& type) {
		return type->iterable();	
	});
}

bool LType::container() const {
	return std::all_of(begin(_types), end(_types), [](auto& type) {
		return type->container();	
	});
}

std::ostream& LType::print(std::ostream& os) const {
	if (_types.size() == 1) {
		os << *_types.begin();
	} else {
		for (auto i = _types.begin(); i != _types.end(); ++i) {
			if (i != _types.begin()) os << " | ";
			os << *i;
		}
	}
	return os;
}

}