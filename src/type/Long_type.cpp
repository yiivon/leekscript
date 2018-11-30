#include "Long_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* Long_type::llvm() const {
	return llvm::Type::getInt64Ty(LLVMCompiler::context);
}
std::ostream& Long_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "long" << END_COLOR;
	return os;
}

}