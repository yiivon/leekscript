#ifndef CONTINUE_HPP
#define CONTINUE_HPP

#include "Instruction.hpp"
#include "../value/Expression.hpp"

class Continue : public Instruction {
public:

	char value;

	Continue();
	virtual ~Continue();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif

