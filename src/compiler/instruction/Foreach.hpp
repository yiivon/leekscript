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
	Value* container;
	Block* body;
	
	Type key_type;
	Type value_type;
	std::shared_ptr<SemanticVar> value_var;
	std::shared_ptr<SemanticVar> key_var;

	Foreach();
	virtual ~Foreach();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyze(SemanticAnalyzer*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
