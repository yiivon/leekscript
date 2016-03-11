#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "VariableDeclaration.hpp"
#include "Instruction.hpp"

class ClassDeclaration : public Instruction {
public:

	std::string name;
	std::vector<VariableDeclaration*> fields;

	ClassDeclaration();
	virtual ~ClassDeclaration();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
