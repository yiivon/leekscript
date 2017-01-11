#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "../../compiler/value/Value.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class SemanticVar;
enum class VarScope;

class Reference : public Value {
public:

	Token* variable;
	SemanticVar* var;
	int capture_index = 0;
	VarScope scope;

	Reference(Token* variable);
	virtual ~Reference();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
