#include "Iterator_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* Iterator_type::integer_iterator = nullptr;
llvm::Type* Iterator_type::long_iterator = nullptr;

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
	if (_container == Type::INTEGER) {
		return get_integer_iterator();
	} else if (_container == Type::LONG) {
		return get_long_iterator();
	} else if (_container == Type::INT_ARRAY) {
		return llvm::Type::getInt32PtrTy(LLVMCompiler::context);
	} else if (_container == Type::REAL_ARRAY) {
		return llvm::Type::getDoublePtrTy(LLVMCompiler::context);
	} else if (_container == Type::PTR_ARRAY) {
		return Any_type::get_any_type()->getPointerTo();
	}
	assert(false && "No LLVM type for this iterator");
}
std::string Iterator_type::clazz() const {
	return "Iterator";
}
std::ostream& Iterator_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "iterator" << END_COLOR << "<" << _container << ">";
	return os;
}

llvm::Type* Iterator_type::get_integer_iterator() {
	if (integer_iterator == nullptr) {
		integer_iterator = llvm::StructType::create("integeriterator",
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context)
		);
	}
	return integer_iterator;
}
llvm::Type* Iterator_type::get_long_iterator() {
	if (long_iterator == nullptr) {
		long_iterator = llvm::StructType::create("longiterator",
			llvm::Type::getInt64Ty(LLVMCompiler::context),
			llvm::Type::getInt64Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context)
		);
	}
	return long_iterator;
}

}