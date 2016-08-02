#ifndef VARIABLEVALUE_HPP
#define VARIABLEVALUE_HPP

#include "../lexical/Ident.hpp"
#include "../value/LeftValue.hpp"
#include "../Compiler.hpp"

namespace ls {

class Value;
class SemanticVar;

class VariableValue : public LeftValue {
public:

	std::string name;
	Token* token;
	SemanticVar* var;

	VariableValue(Token* token);
	virtual ~VariableValue();

	virtual void print(std::ostream&) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;
	void must_return(SemanticAnalyser* analyser, const Type& type) override;
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
	virtual jit_value_t compile_jit_l(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
