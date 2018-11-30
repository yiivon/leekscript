#include "Map_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>

namespace ls {

Type Map_type::key() const {
	return _key;
}
Type Map_type::element() const {
	return _element;
}
bool Map_type::operator == (const Base_type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element == map->_element && _key == map->_key;
	}
	return false;
}
bool Map_type::compatible(const Base_type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element.compatible(map->_element) && _key.compatible(map->_key);
	}
	return false;
}
llvm::Type* Map_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}

}