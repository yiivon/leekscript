#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "Instruction.hpp"

namespace ls {

class ExpressionInstruction : public Instruction {
public:

	Value* value;

	ExpressionInstruction(Value* expression);
	virtual ~ExpressionInstruction();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type& type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
