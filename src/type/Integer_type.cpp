#include "Integer_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

Type Integer_type::element() const {
	return Type::INTEGER;
}
Type Integer_type::iterator() const {
	return Type::INTEGER_ITERATOR;
}
bool Integer_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Integer_type*>(type);
}
llvm::Type* Integer_type::llvm() const {
	return llvm::Type::getInt32Ty(LLVMCompiler::context);
}
std::ostream& Integer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "int" << END_COLOR;
	return os;
}

}