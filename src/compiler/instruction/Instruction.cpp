#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

Instruction::~Instruction() {}

bool Instruction::can_return() const {
	return false;
}

Compiler::value Instruction::compile(Compiler&) const {
	assert(false && "Dont compile default instruction");
}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
