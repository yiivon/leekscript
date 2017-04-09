#ifndef ABSOLUTEVALUE_HPP
#define ABSOLUTEVALUE_HPP

#include "Value.hpp"

namespace ls {

class AbsoluteValue : public Value {
public:

	Value* expression;

	AbsoluteValue();
	virtual ~AbsoluteValue();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
