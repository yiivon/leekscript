#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <ostream>
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Value;
class SemanticAnalyzer;

class Instruction {
public:

	Type type;
	Type return_type;
	bool returning = false;
	bool may_return = false;
	bool is_void = false;
	bool throws = false;

	virtual ~Instruction() = 0;

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const = 0;
	virtual Location location() const = 0;

	virtual void analyze(SemanticAnalyzer* analyzer, const Type& type = Type::any()) = 0;

	virtual Compiler::value compile(Compiler&) const;

	virtual Instruction* clone() const = 0;

	std::string tabs(int indent) const;
};

}

#endif
