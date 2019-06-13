#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>

#include "Instruction.hpp"
#include "VariableDeclaration.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class LSClass;

class ClassDeclaration : public Instruction {
public:

	std::shared_ptr<Token> token;
	std::string name;
	std::vector<VariableDeclaration*> fields;
	Variable* var;

	ClassDeclaration(std::shared_ptr<Token> token);
	virtual ~ClassDeclaration();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
