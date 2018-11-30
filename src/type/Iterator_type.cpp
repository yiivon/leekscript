#include "Iterator_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

bool Iterator_type::operator == (const BaseRawType* type) const {
	if (auto array = dynamic_cast<const Iterator_type*>(type)) {
		return _container == array->_container;
	}
	return false;
}
bool Iterator_type::compatible(const BaseRawType* type) const {
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
// std::ostream& Array_type::print(std::ostream& os) const {
// 	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
// 	return os;
// }

}