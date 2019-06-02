#include <iostream>
#include "Map_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Map_type::Map_type(const Type* key, const Type* element) : Pointer_type(Type::structure("map", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::boolean, // native
	element->pointer(),
	element->pointer(),
	element->pointer(),
	Type::structure("map_node", {
		Type::long_, Type::long_, Type::long_, Type::long_,
		element	
	})->pointer()
})), _key(key), _element(element) {}

const Type* Map_type::key() const {
	return _key;
}
const Type* Map_type::element() const {
	return _element;
}
const Type* Map_type::iterator() const {
	const auto key_merged = _key->fold();
	const auto element_merged = _element->fold();
	return Type::structure("map_node", {
		Type::long_, Type::long_, Type::long_, Type::long_, key_merged, element_merged
	})->pointer();
}
bool Map_type::operator == (const Base_type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element == map->_element && _key == map->_key;
	}
	return false;
}
bool Map_type::compatible(const Base_type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element->compatible(map->_element) && _key->compatible(map->_key);
	}
	return false;
}
int Map_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1000; }
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		if (map->element()->is_void() or map->key()->is_void()) {
			return 999;
		}
		return _element->distance(map->_element) + _key->distance(map->_key);
	}
	return -1;
}
std::string Map_type::clazz() const {
	return "Map";
}
std::ostream& Map_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "map" << END_COLOR << "<" << _key << ", " << _element << ">";
	return os;
}

}