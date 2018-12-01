#include "Iterator_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

bool Iterator_type::operator == (const Base_type* type) const {
	if (auto array = dynamic_cast<const Iterator_type*>(type)) {
		return _container == array->_container;
	}
	return false;
}
bool Iterator_type::compatible(const Base_type* type) const {
	if (auto array = dynamic_cast<const Iterator_type*>(type)) {
		return _container.compatible(array->_container);
	}
	return false;
}
llvm::Type* Iterator_type::llvm() const {
	if (_container == Type::INT_ARRAY_ITERATOR) {
		return llvm::Type::getInt32PtrTy(LLVMCompiler::context);
	}
	if (_container == Type::PTR_ARRAY_ITERATOR) {
		return Type::LLVM_LSVALUE_TYPE_PTR->getPointerTo();
	}
}
std::string Iterator_type::clazz() const {
	return "Iterator";
}
std::ostream& Iterator_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "iterator" << END_COLOR << "<" << _container << ">";
	return os;
}

}