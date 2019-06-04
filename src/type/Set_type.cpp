#include "Set_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

std::map<const Type*, const Set_type*> Set_type::cache;
std::map<const Type*, const Type*> Set_type::nodes;
std::map<const Type*, const Type*> Set_type::iterators;

const Set_type* Set_type::create(const Type* element) {
	auto i = cache.find(element->not_temporary());
	if (i != cache.end()) {
		return i->second;
	} else {
		const auto a = new Set_type(element);
		cache.insert({element->not_temporary(), a});
		return a;
	}
}

Set_type::Set_type(const Type* element) : Pointer_type(Type::structure("set", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::boolean, // native
	element->pointer(),
	element->pointer(),
	element->pointer(),
	get_node_type(element)
})), _element(element) {}

const Type* Set_type::key() const {
	return Type::integer;
}
const Type* Set_type::element() const {
	return _element;
}
const Type* Set_type::iterator() const {
	const auto merged = _element->fold();
	return get_iterator(merged);
}
bool Set_type::operator == (const Base_type* type) const {
	if (auto array = dynamic_cast<const Set_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
int Set_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1000; }
	if (auto set = dynamic_cast<const Set_type*>(type)) {
		if (set->_element->is_void()) {
			return 999;
		}
		return _element->distance(set->_element);
	}
	return -1;
}
std::string Set_type::clazz() const {
	return "Set";
}
std::ostream& Set_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "set" << END_COLOR << "<" << _element << ">";
	return os;
}

const Type* Set_type::get_iterator(const Type* element) {
	auto i = iterators.find(element);
	if (i != iterators.end()) return i->second;
	auto type = Type::structure("set_iterator", {
		get_node_type(element),
		Type::integer
	});
	iterators.insert({ element, type });
	return type;
}

const Type* Set_type::get_node_type(const Type* element) {
	auto i = nodes.find(element);
	if (i != nodes.end()) return i->second;
	auto type = Type::structure("set_node", {
		Type::long_, Type::long_, Type::long_, Type::long_,
		element
	})->pointer();
	nodes.insert({ element, type });
	return type;
}

}