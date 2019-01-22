#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>
#include "../instruction/Instruction.hpp"
#include "Value.hpp"

namespace ls {

class Block : public Value {
public:

	std::vector<Instruction*> instructions;
	bool temporary_mpz = false;
	bool was_reference = false;

	Block();
	virtual ~Block();

	virtual void print(std::ostream&, int indent = 0, bool debug = false, bool condensed = true) const override;
	virtual Location location() const override;

	void analyse_global_functions(SemanticAnalyser* analyser);
	virtual void analyse(SemanticAnalyser* analyser) override;

	Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
