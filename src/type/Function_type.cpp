#include "Function_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Placeholder_type.hpp"

namespace ls {

Function_type::Function_type(const Type& ret, const std::vector<Type>& args, bool closure, const Function* function) : _return_type(ret), _arguments(args), _closure(closure), _function(function) {}

bool Function_type::operator == (const Base_type* type) const {
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		return _return_type == fun->_return_type && _arguments == fun->_arguments;
	}
	return false;
}
bool Function_type::compatible(const Base_type* type) const {
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		if (_closure != fun->_closure or _arguments.size() < fun->_arguments.size()) return false;
		return _return_type.compatible(fun->_return_type);
	}
	return false;
}
Type Function_type::return_type() const {
	return _return_type;
}
std::vector<Type> Function_type::arguments() const {
	return _arguments;
}
Type Function_type::argument(size_t i) const {
	// assert(i < _arguments.size() && "Wrong argument() index");
	if (i < _arguments.size()) {
		return _arguments[i];
	}
	return Type::ANY;
}
llvm::Type* Function_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}
std::string Function_type::clazz() const {
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

}