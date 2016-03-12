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
	vector<string> variables;
	vector<Value*> expressions;
	map<string, SemanticVar*> vars;
	bool return_value;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
