#ifndef BREAK_HPP
#define BREAK_HPP

#include <memory>
#include "Instruction.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Break : public Instruction {
public:

	std::unique_ptr<Token> token;
	int deepness;

	Break();
	virtual ~Break();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
