#include "Real_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Real_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Real_type*>(type);
}
llvm::Type* Real_type::llvm() const {
	return llvm::Type::getDoubleTy(LLVMCompiler::context);
}
std::ostream& Real_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "real" << END_COLOR;
	return os;
}

}