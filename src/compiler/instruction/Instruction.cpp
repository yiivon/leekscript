#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

void Instruction::pre_analyze(SemanticAnalyzer*) {}

Compiler::value Instruction::compile(Compiler&) const {
	assert(false && "Dont compile default instruction");
}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
