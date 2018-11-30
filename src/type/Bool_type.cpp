#include "Bool_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* Bool_type::llvm() const {
	return llvm::Type::getInt1Ty(LLVMCompiler::context);
}
std::ostream& Bool_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "bool" << END_COLOR;
	return os;
}

}