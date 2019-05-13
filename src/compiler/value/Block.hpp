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
	bool mpz_pointer = false;
	bool was_reference = false;

	Block();
	virtual ~Block();

	virtual void print(std::ostream&, int indent = 0, bool debug = false, bool condensed = false) const override;
	virtual Location location() const override;

	void analyze_global_functions(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer) override;

	Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
