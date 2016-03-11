#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>

#include "Value.hpp"

class FunctionCall : public Value {
public:

	Value* function;
	std::vector<Value*> arguments;

	bool is_native = false;
	string native_func;

	void* std_func;
	Value* this_ptr;

	FunctionCall();
	virtual ~FunctionCall();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
