#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "../lexical/Token.hpp"
#include "Value.hpp"

class Number : public Value {
public:

	double value;

	Number(double value);
	virtual ~Number();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
