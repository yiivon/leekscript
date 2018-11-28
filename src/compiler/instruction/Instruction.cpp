#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

Instruction::~Instruction() {}

bool Instruction::can_return() const {
	return false;
}

Compiler::value Instruction::compile(Compiler&) const {}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
