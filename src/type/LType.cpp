#include "LType.hpp"
#include <algorithm>
#include "../colors.h"

namespace ls {

LType::LType() {}

std::vector<std::shared_ptr<Base_type>> LType::types() const {
	return _types;
}

void LType::add(const std::shared_ptr<Base_type> type) {
	_types.push_back(type);
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