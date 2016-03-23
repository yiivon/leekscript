#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>
#include "Value.hpp"
#include "LeftValue.hpp"

class ObjectAccess : public LeftValue {
public:

	Value* object;
	std::string field;
	bool class_attr = false;
	void* attr_addr;

	ObjectAccess();
	virtual ~ObjectAccess();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;

	virtual jit_value_t compile_jit_l(Compiler&, jit_function_t&, Type) const override;
};

#endif
