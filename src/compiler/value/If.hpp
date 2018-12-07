#ifndef IF_HPP_
#define IF_HPP_

#include "Expression.hpp"
#include "../value/Block.hpp"

namespace ls {

class If : public Value {
public:

	Value* condition;
	Block* then;
	Block* elze;
	bool ternary;
	bool returning = false;
	bool may_return = false;

	If(bool ternary = false);
	virtual ~If();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
