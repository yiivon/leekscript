#ifndef RETURN_HPP
#define RETURN_HPP

#include "Instruction.hpp"
#include "../value/Expression.hpp"
#include "../value/Function.hpp"

class Return : public Instruction {
public:

	Value* expression;
	Function* function;
	bool in_function;

	Return();
	Return(Value*);
	virtual ~Return();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
