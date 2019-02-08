#ifndef THROW_HPP
#define THROW_HPP

#include "Instruction.hpp"

namespace ls {

class Throw : public Instruction {
public:

	std::shared_ptr<Token> token;
	Value* expression;

	Throw(std::shared_ptr<Token> token, Value* = nullptr);
	virtual ~Throw();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
