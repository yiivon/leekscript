#include "Array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"

namespace ls {

std::map<const Type*, std::shared_ptr<Array_type>> Array_type::cache;

std::shared_ptr<Array_type> Array_type::create(const Type* element) {
	auto i = cache.find(element->not_temporary());
	if (i != cache.end()) {
		return i->second;
	} else {
		const auto a = std::make_shared<Array_type>(element);
		cache.insert({element->not_temporary(), a});
		return a;
	}
}

Array_type::Array_type(const Type* element) : Pointer_type(Type::structure("array<" + element->getJsonName() + ">", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // refs
	Type::boolean, // native
	element->pointer(), // vector.begin
	element->pointer(), // vector.end
	element->pointer() // vector.data
})), _element(element) {
}

const Type* Array_type::key() const {
	return Type::integer;
}
const Type* Array_type::element() const {
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
		return _element->is_void() or _element->fold() == array->_element->fold();
	}
	return false;
}
int Array_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1000; }
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		if (array->_element->is_void()) {
			return 999;
		}
		return _element->fold() == array->_element->fold() ? 0 : -1;
	}
	return -1;
}
const Type* Array_type::iterator() const {
	const auto merged = _element->fold();
	if (merged->is_integer()) return Type::integer->pointer();
	if (merged->is_real()) return Type::real->pointer();
	return Type::any->pointer();
}
std::string Array_type::clazz() const {
	return "Array";
}
std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
	return os;
}

}