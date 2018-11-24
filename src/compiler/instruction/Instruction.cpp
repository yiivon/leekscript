#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

Instruction::~Instruction() {}

Compiler::value Instruction::compile(Compiler&) const {}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
