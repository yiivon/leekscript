#include "Set_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Set_type::Set_type(Type element) : Pointer_type(Type {
	new Struct_type(std::string("_set"), {
		Type::INTEGER, // ?
		Type::INTEGER, // ?
		Type::INTEGER, // ?
		Type::INTEGER, // ?
		Type::BOOLEAN, // native
		element.pointer(),
		element.pointer(),
		element.pointer(),
		Type({ new Struct_type("set_node", {
			Type::LONG, Type::LONG,	Type::LONG,	Type::LONG,
			element	
		}) }).pointer()
	})
}), _element(element) {}

Type Set_type::element() const {
	return _element;
}
Type Set_type::iterator() const {
	const auto merged = _element.fold();
	if (merged.is_integer()) return Type::INT_SET_ITERATOR;
	if (merged.is_real()) return Type::REAL_SET_ITERATOR;
	return Type::PTR_SET_ITERATOR;
}
bool Set_type::operator == (const Base_type* type) const {
	if (auto array = dynamic_cast<const Set_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
bool Set_type::compatible(const Base_type* type) const {
	if (auto set = dynamic_cast<const Set_type*>(type)) {
		return _element.compatible(set->_element);
	}
	return false;
}
std::string Set_type::clazz() const {
	return "Set";
}
std::ostream& Set_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
	return os;
}

}