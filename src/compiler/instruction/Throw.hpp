#ifndef THROW_HPP
#define THROW_HPP

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/value/Function.hpp"

namespace ls {

class Throw : public Instruction {
public:

	Token* token;
	Value* expression;

	Throw(Token* token, Value* = nullptr);
	virtual ~Throw();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
