#ifndef PREFIXEXPRESSION_HPP
#define PREFIXEXPRESSION_HPP

#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/Value.hpp"
#include "../lexical/Operator.hpp"

namespace ls {

class PrefixExpression : public Value {
public:

	Operator* operatorr;
	Value* expression;

	PrefixExpression();
	virtual ~PrefixExpression();

	virtual void print(std::ostream&) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
