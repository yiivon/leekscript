#ifndef WHILE_HPP
#define WHILE_HPP

#include <vector>
#include "../../compiler/value/Value.hpp"
#include "../value/Block.hpp"
class Variable;

namespace ls {

class While : public Instruction {
public:

	std::unique_ptr<Value> condition;
	std::unique_ptr<Block> body;
	Token* token;

	While();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
