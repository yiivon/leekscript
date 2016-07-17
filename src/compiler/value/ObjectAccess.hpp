#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>

#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class ObjectAccess : public LeftValue {
public:

	Value* object;
	Token* field;
	std::string object_class;
	std::string class_name;
	bool class_attr = false;
	void* attr_addr;
	void* access_function = nullptr;

	ObjectAccess();
	virtual ~ObjectAccess();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;

	virtual jit_value_t compile_jit_l(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
