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
	Reference(Token* variable);
	virtual ~Reference();

	virtual void print(std::ostream&, bool debug) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
