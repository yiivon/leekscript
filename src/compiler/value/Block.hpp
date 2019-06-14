#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>
#include "../instruction/Instruction.hpp"
#include "Value.hpp"

namespace ls {

class Block : public Value {
public:

	bool is_function_block = false;
	std::vector<std::unique_ptr<Instruction>> instructions;
	bool temporary_mpz = false;
	bool mpz_pointer = false;
	bool was_reference = false;

	Block(bool is_function_block = false);

	virtual void print(std::ostream&, int indent = 0, bool debug = false, bool condensed = false) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer* analyzer) override;

	Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
