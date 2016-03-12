#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>
#include "Value.hpp"
#include "../lexical/Operator.hpp"

class Expression : public Value {
public:

	Value* v1;
	Value* v2;
	Operator* op;

	bool fast;
	bool ignorev2;
	bool no_op;

	Expression();
	Expression(Value*);
	virtual ~Expression();

	void append(Operator*, Value*);

	void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
