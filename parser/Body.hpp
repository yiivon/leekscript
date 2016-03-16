#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include "instruction/Instruction.hpp"
#include "value/Value.hpp"
#include "../Compiler.hpp"

class Body {
public:

	std::vector<Instruction*> instructions;
	Type type;

	Body();
	virtual ~Body();

	void print(std::ostream& os);

	void analyse(SemanticAnalyser* analyser, const Type& req_type);

	jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const;
};

#endif
