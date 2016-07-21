#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "../../compiler/value/Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Number : public Value {
public:

	Token* token;
	double value;

	Number(double value, Token* token);
	virtual ~Number();

	virtual void print(std::ostream&) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
