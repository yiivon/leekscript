#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Expression.hpp"
#include "../lexical/Ident.hpp"
#include "../../vm/VM.hpp"

namespace ls {

class SemanticVar;

class VariableDeclaration : public Instruction {
public:

	bool global;
	std::vector<Token*> variables;
	std::vector<Value*> expressions;
	std::map<std::string, SemanticVar*> vars;
	bool return_value;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(std::ostream&, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
