#ifndef CONTINUE_HPP
#define CONTINUE_HPP

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/value/Expression.hpp"

namespace ls {

class Continue : public Instruction {
public:

	char value;

	Continue();
	virtual ~Continue();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif

