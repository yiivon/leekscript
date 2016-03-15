#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "Instruction.hpp"
#include "../value/Expression.hpp"

class ExpressionInstruction : public Instruction {
public:

	Value* value;

	ExpressionInstruction(Value* expression);
	virtual ~ExpressionInstruction();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
