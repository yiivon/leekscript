#include "Array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"

namespace ls {

std::map<Type, std::shared_ptr<Array_type>> Array_type::cache;

std::shared_ptr<Array_type> Array_type::create(Type element) {
	auto i = cache.find(element);
	if (i != cache.end()) {
		return i->second;
	} else {
		const auto a = std::make_shared<Array_type>(element);
		cache.insert({element, a});
		return a;
	}
}

Array_type::Array_type(Type element) : Pointer_type(Type { std::make_shared<const Struct_type>(std::string("array<" + element.getJsonName() + ">"), std::initializer_list<Type> {
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // ?
	Type::integer(), // refs
	Type::boolean(), // native
	element.pointer(), // vector.begin
	element.pointer(), // vector.end
	element.pointer() // vector.data
}) }), _element(element) {
}

Type Array_type::key() const {
	return Type::integer();
}
Type Array_type::element() const {
	return _element;
}
bool Array_type::operator == (const Base_type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
bool Array_type::compatible(const Base_type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element._types.size() == 0 or _element.fold() == array->_element.fold();
	}
	return false;
}
int Array_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1000; }
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		if (array->_element._types.size() == 0) {
			return 999;
		}
		return _element.distance(array->_element);
	}
	return -1;
}
Type Array_type::iterator() const {
	const auto merged = _element.fold();
	if (merged.is_integer()) return Type::integer().pointer();
	if (merged.is_real()) return Type::real().pointer();
	return Type::any().pointer();
}
std::string Array_type::clazz() const {
	return "Array";
}
std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
	return os;
}

}