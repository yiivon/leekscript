#include "Long_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

Type Long_type::element() const {
	return Type::INTEGER;
}
Type Long_type::iterator() const {
	return Type::LONG_ITERATOR;
}
bool Long_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Long_type*>(type);
}
llvm::Type* Long_type::llvm() const {
	return llvm::Type::getInt64Ty(LLVMCompiler::context);
}
std::ostream& Long_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "long" << END_COLOR;
	return os;
}

}