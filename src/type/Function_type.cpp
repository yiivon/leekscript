#include "Function_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Placeholder_type.hpp"
#include "Any_type.hpp"

namespace ls {

llvm::Type* Function_type::function_type = nullptr;

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
bool Function_type::castable(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type) != nullptr) { return true; }
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		if (_closure != fun->_closure or _arguments.size() > fun->_arguments.size()) return false;
		return fun->_return_type.castable(_return_type);
	}
	return false;
}
int Function_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		if (_closure != fun->_closure or _arguments.size() > fun->_arguments.size()) return -1;
		return fun->_return_type.distance(_return_type);
	}
	return -1;
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
	return get_function_type();
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

llvm::Type* Function_type::get_function_type() {
	if (function_type == nullptr) {
		function_type = llvm::StructType::create("lsfunction",
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt1Ty(LLVMCompiler::context),
			llvm::Type::getInt64Ty(LLVMCompiler::context)->getPointerTo()
		)->getPointerTo();
	}
	return function_type;
}



}