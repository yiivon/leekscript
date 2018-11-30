#include "Real_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* FloatRawType::llvm() const {
	return llvm::Type::getDoubleTy(LLVMCompiler::context);
}

int Real_type::_id = Ty::get_next_id();

Real_type::Real_type(int id, const std::string name) : Number_type(_id * id, name) {}
Real_type::~Real_type() {}

std::ostream& Real_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "real" << END_COLOR;
	return os;
}

}