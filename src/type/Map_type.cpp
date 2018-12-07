#include <iostream>
#include "Map_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include "Any_type.hpp"

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
std::string Map_type::clazz() const {
	return "Map";
}
llvm::Type* Map_type::llvm() const {
	return Any_type::get_any_type();
}
std::ostream& Map_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "map" << END_COLOR << "<" << _key << ", " << _element << ">";
	return os;
}

}