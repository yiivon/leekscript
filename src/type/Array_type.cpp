#include "Array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "../compiler/LLVMCompiler.hpp"

namespace ls {

llvm::Type* Array_type::any_array_type = nullptr;
llvm::Type* Array_type::int_array_type = nullptr;
llvm::Type* Array_type::real_array_type = nullptr;

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
		// TODO no need to fold the element types
		// return _element.fold().compatible(array->_element.fold());
		return _element._types.size() == 0 or _element.fold() == array->_element.fold();
	}
	return false;
}
llvm::Type* Array_type::llvm() const {
	const auto merged = _element.fold();
	if (merged.is_integer()) {
		return get_int_array_type();
	} else if (merged.is_real()) {
		return get_real_array_type();
	} else {
		return get_any_array_type();
	}
}
Type Array_type::iterator() const {
	const auto merged = _element.fold();
	if (merged.is_integer()) return Type::INT_ARRAY_ITERATOR;
	if (merged.is_real()) return Type::REAL_ARRAY_ITERATOR;
	return Type::PTR_ARRAY_ITERATOR;
}
std::string Array_type::clazz() const {
	return "Array";
}
std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR << "<" << _element << ">";
	return os;
}

llvm::Type* Array_type::get_any_array_type() {
	if (any_array_type == nullptr) {
		any_array_type = llvm::StructType::create("lsarray_any",
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt1Ty(LLVMCompiler::context),
			Type::LLVM_LSVALUE_TYPE_PTR_PTR,
			Type::LLVM_LSVALUE_TYPE_PTR_PTR,
			Type::LLVM_LSVALUE_TYPE_PTR_PTR
		)->getPointerTo();
	}
	return any_array_type;
}
llvm::Type* Array_type::get_int_array_type() {
	if (int_array_type == nullptr) {
		int_array_type = llvm::StructType::create("lsarray_int",
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt1Ty(LLVMCompiler::context),
			llvm::Type::getInt32PtrTy(LLVMCompiler::context),
			llvm::Type::getInt32PtrTy(LLVMCompiler::context),
			llvm::Type::getInt32PtrTy(LLVMCompiler::context)
		)->getPointerTo();
	}
	return int_array_type;
}
llvm::Type* Array_type::get_real_array_type() {
	if (real_array_type == nullptr) {
		real_array_type = llvm::StructType::create("lsarray_real",
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt32Ty(LLVMCompiler::context),
			llvm::Type::getInt1Ty(LLVMCompiler::context),
			llvm::Type::getDoublePtrTy(LLVMCompiler::context),
			llvm::Type::getDoublePtrTy(LLVMCompiler::context),
			llvm::Type::getDoublePtrTy(LLVMCompiler::context)
		)->getPointerTo();
	}
	return real_array_type;
}

}