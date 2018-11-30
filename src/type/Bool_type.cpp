#include "Bool_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* BooleanRawType::llvm() const {
	return llvm::Type::getInt1Ty(LLVMCompiler::context);
}

int Bool_type::_id = Ty::get_next_id();

Bool_type::Bool_type() : Base_type(_id, "bool") {}
Bool_type::~Bool_type() {}

std::ostream& Bool_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "bool" << END_COLOR;
	return os;
}

}