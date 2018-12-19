#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* Any_type::any_type = nullptr;

bool Any_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Any_type*>(type);
}
int Any_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 0; }
	return 100000;
}
llvm::Type* Any_type::llvm() const {
	return Any_type::get_any_type();
}
std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}

llvm::Type* Any_type::get_any_type() {
	if (any_type == nullptr) {
		any_type = llvm::StructType::create("lsvalue", llvm::Type::getInt32Ty(Compiler::context), llvm::Type::getInt32Ty(Compiler::context), llvm::Type::getInt32Ty(Compiler::context), llvm::Type::getInt32Ty(Compiler::context), llvm::Type::getInt1Ty(Compiler::context))->getPointerTo();
	}
	return any_type;
}

}