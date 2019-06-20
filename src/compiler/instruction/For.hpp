#ifndef FOR_HPP
#define FOR_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "Instruction.hpp"

namespace ls {

class Block;
class Variable;

class For : public Instruction {
public:

	std::shared_ptr<Token> token;
	std::unique_ptr<Block> init;
	std::unique_ptr<Value> condition;
	std::unique_ptr<Block> increment;
	std::unique_ptr<Block> body;

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
