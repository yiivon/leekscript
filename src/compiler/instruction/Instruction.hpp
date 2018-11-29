#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <ostream>
#include "../Compiler.hpp"
#include "../../type/Ty.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Value;
class SemanticAnalyser;

class Instruction {
public:

	Type type;

	virtual ~Instruction() = 0;

	virtual void print(std::ostream&, int indent, bool debug) const = 0;
	virtual Location location() const = 0;

	virtual void analyse(SemanticAnalyser* analyser, const Type& type = Type::POINTER) = 0;
	virtual bool can_return() const;

	virtual Compiler::value compile(Compiler&) const;

	virtual Instruction* clone() const = 0;

	std::string tabs(int indent) const;
};

}

#endif
