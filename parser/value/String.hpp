#ifndef STRING_HPP_
#define STRING_HPP_

#include <string>
#include "../lexical/Token.hpp"
#include "Value.hpp"

class String : public Value {
public:

	string value;

	String(string value);
	virtual ~String();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
