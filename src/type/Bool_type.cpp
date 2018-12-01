#include "Bool_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Bool_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Bool_type*>(type);
}
llvm::Type* Bool_type::llvm() const {
	return llvm::Type::getInt1Ty(LLVMCompiler::context);
}
std::string Bool_type::clazz() const {
	return "Boolean";
}
std::ostream& Bool_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "bool" << END_COLOR;
	return os;
}

}