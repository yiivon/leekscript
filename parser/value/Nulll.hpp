#ifndef NULLL_HPP
#define NULLL_HPP

#include "Value.hpp"

class Nulll : public Value {
public:
	Nulll();
	virtual ~Nulll();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
