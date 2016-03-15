#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "../value/Expression.hpp"
#include "Instruction.hpp"
#include "../../vm/VM.hpp"
#include "../semantic/SemanticAnalyser.hpp"
class SemanticVar;

class VariableDeclaration : public Instruction {
public:

	bool global;
	std::vector<std::string> variables;
	std::vector<Value*> expressions;
	std::map<std::string, SemanticVar*> vars;
	bool return_value;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
