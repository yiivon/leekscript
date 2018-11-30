#include "Integer_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"

namespace ls {

llvm::Type* IntegerRawType::llvm() const {
	return llvm::Type::getInt32Ty(LLVMCompiler::context);
}
std::ostream& IntegerRawType::print(std::ostream& os) const {
	os << BLUE_BOLD << "int" << END_COLOR;
	return os;
}

int Integer_type::_id = Ty::get_next_id();

Integer_type::Integer_type(int id, const std::string name) : Number_type(_id * id, name) {}
Integer_type::~Integer_type() {}

std::ostream& Integer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "integer" << END_COLOR;
	return os;
}

}