#ifndef POSTFIXEXPRESSION_HPP
#define POSTFIXEXPRESSION_HPP

#include "Value.hpp"
#include "Expression.hpp"
#include "LeftValue.hpp"

class PostfixExpression : public Value {
public:

	LeftValue* expression;
	Operator* operatorr;
	bool return_value;

	PostfixExpression();
	virtual ~PostfixExpression();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
