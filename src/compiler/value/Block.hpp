#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>
#include "../instruction/Instruction.hpp"
#include "Value.hpp"

namespace ls {

class Block : public Value {
public:

	std::vector<Instruction*> instructions;
	Type type;

	Block();
	virtual ~Block();

	virtual void print(std::ostream&, bool debug) const override;
	virtual int line() const override;

	void analyse(SemanticAnalyser* analyser, const Type& req_type);

	jit_value_t compile(Compiler&) const;

};

}

#endif
