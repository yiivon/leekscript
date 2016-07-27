#ifndef LEFTVALUE_HPP_
#define LEFTVALUE_HPP_

#include "Value.hpp"
#include "../Compiler.hpp"

namespace ls {

class LeftValue : public Value {
public:

	LeftValue();
	virtual ~LeftValue();

	virtual bool isLeftValue() const override;
	virtual void change_type(SemanticAnalyser*, const Type&) = 0;

	virtual jit_value_t compile_jit_l(Compiler&, jit_function_t&, Type) const = 0;
};

}

#endif
