#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>
#include "Value.hpp"
#include "LeftValue.hpp"

class ObjectAccess : public LeftValue {
public:

	Value* object;
	string field;

	bool map = false;

	ObjectAccess();
	virtual ~ObjectAccess();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;

	virtual jit_value_t compile_jit_l(Compiler&, jit_function_t&, Type) const override;
};

#endif
