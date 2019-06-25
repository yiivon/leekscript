#include "Phi.hpp"
#include "../value/Block.hpp"
#include "../semantic/Variable.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/FunctionVersion.hpp"

namespace ls {

Phi::Phi(Variable* variable, Block* block1, Variable* variable1, Block* block2, Variable* variable2) : variable(variable), block1(block1), variable1(variable1), block2(block2), variable2(variable2) {}

std::vector<Phi*> Phi::build_phis(SemanticAnalyzer* analyzer, Block* block1, Block* block2) {
	std::vector<Phi*> phis;
	for (const auto& variable : block1->variables) {
		auto i = block2->variables.find(variable.second->name);
		if (i != block2->variables.end()) {
			if (variable.second->parent == i->second->parent or variable.second == i->second->parent) {
				auto first_block_var = variable.second;
				auto new_var = analyzer->update_var(i->second->parent);
				auto phi = new Phi { new_var, block1, first_block_var, block2, i->second };
				phis.push_back(phi);
				first_block_var->phi = phi;
				i->second->phi = phi;
			}
		}
	}
	return phis;
}

}