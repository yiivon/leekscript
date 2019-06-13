#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>
#include "Instruction.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

class Variable;

class VariableDeclaration : public Instruction {
public:

	bool global;
	bool constant;
	bool function;
	std::vector<std::shared_ptr<Token>> variables;
	std::vector<Value*> expressions;
	std::map<std::string, Variable*> vars;
	std::shared_ptr<Token> keyword;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
