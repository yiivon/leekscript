#ifndef IF_HPP_
#define IF_HPP_

#include "Expression.hpp"
#include "../Body.hpp"

class If : public Value {
public:

	Value* condition;
	Body* then;
	Body* elze;
	bool inversed;

	If();
	virtual ~If();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
