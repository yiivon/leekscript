#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "Value.hpp"
#include "../lexical/Ident.hpp"

class Reference : public Value {
public:

	std::string variable;

	Reference();
	virtual ~Reference();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
