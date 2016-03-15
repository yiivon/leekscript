#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <vector>
#include "Expression.hpp"

class Array : public Value {
public:

	bool associative = false;
	std::vector<Value*> keys;
	std::vector<Value*> expressions;
	bool only_values = true;

	Array();
	virtual ~Array();

	void addValue(Value* value, Value* key);

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	void elements_will_take(SemanticAnalyser*, const unsigned, const Type&, int level);

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
