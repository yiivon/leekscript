#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "../../compiler/value/Value.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class SemanticVar;

class Reference : public Value {
public:

	Token* variable;
	SemanticVar* var;

	Reference();
	virtual ~Reference();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
