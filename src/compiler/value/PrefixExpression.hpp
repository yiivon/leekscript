#ifndef PREFIXEXPRESSION_HPP
#define PREFIXEXPRESSION_HPP

#include "Expression.hpp"
#include "Value.hpp"
#include "../lexical/Operator.hpp"

namespace ls {

class PrefixExpression : public Value {
public:

	std::shared_ptr<Operator> operatorr;
	Value* expression;

	PrefixExpression();
	virtual ~PrefixExpression();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
