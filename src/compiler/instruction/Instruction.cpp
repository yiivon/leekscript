#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

Instruction::~Instruction() {}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
