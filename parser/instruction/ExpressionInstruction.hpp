#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "Instruction.hpp"
#include "../value/Expression.hpp"

namespace ls {

class ExpressionInstruction : public Instruction {
public:

	Value* value;
	bool ret;

	ExpressionInstruction(Value* expression);
	virtual ~ExpressionInstruction();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
