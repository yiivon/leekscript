#include "Function_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Placeholder_type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Function_type::Function_type(const Type* ret, const std::vector<const Type*>& args, bool closure, const Value* function) : Pointer_type(Type::structure("function", {
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // ?
	Type::integer, // refs
	Type::boolean, // native
	Type::long_->pointer() // pointer to the function
}), true), _return_type(ret), _arguments(args), _closure(closure), _function(function) {}

bool Function_type::operator == (const Type* type) const {
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		return _return_type == fun->_return_type && _arguments == fun->_arguments;
	}
	return false;
}
int Function_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (auto fun = dynamic_cast<const Function_type*>(type->folded)) {
		if (_arguments.size() > fun->_arguments.size()) return -1;
		int d = 0;
		for (size_t i = 0; i < _arguments.size(); ++i) {
			d += _arguments.at(i)->distance(fun->_arguments.at(i)->not_temporary());
		}
		int d2 = (_closure != fun->_closure) ? 1 : 0;
		if (d == 0) {
			return d2 + _return_type->distance(fun->_return_type);
		} else {
			return d + d2;
		}
	}
	return -1;
}
const Type* Function_type::return_type() const {
	return _return_type;
}
const std::vector<const Type*>& Function_type::arguments() const {
	return _arguments;
}
const Type* Function_type::argument(size_t i) const {
	// assert(i < _arguments.size() && "Wrong argument() index");
	if (i < _arguments.size()) {
		return _arguments[i];
	}
	return Type::any;
}
std::string Function_type::class_name() const {
	return "Function";
}
std::ostream& Function_type::print(std::ostream& os) const {
	os << BLUE_BOLD << (_closure ? "closure(" : "fun(") << END_COLOR;
	for (unsigned t = 0; t < _arguments.size(); ++t) {
		if (t > 0) os << ", ";
		os << _arguments[t];
	}
	os << BLUE_BOLD << ") => " << _return_type;
	return os;
}
Type* Function_type::clone() const {
	return new Function_type { _return_type, _arguments, _closure, _function };
}

}