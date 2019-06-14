#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

class Foreach : public Instruction {
public:

	std::shared_ptr<Token> key;
	std::shared_ptr<Token> value;
	std::unique_ptr<Value> container;
	std::unique_ptr<Block> body;
	
	const Type* key_type;
	const Type* value_type;
	Variable* value_var;
	Variable* key_var;

	Foreach();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
