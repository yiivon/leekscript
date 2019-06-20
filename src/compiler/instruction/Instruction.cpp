#include "Instruction.hpp"

namespace ls {

void Instruction::pre_analyze(SemanticAnalyzer*) {}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
