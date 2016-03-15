#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include "Value.hpp"

class Boolean : public Value {
public:

	bool value;

	Boolean(bool value);
	virtual ~Boolean();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
