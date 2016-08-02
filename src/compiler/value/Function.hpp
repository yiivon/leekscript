#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>

#include "../../compiler/lexical/Ident.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Value.hpp"
#include "../value/Block.hpp"

namespace ls {

class SemanticVar;

class Function : public Value {
public:

	bool lambda = false;
	std::vector<Token*> arguments;
	std::vector<bool> references;
	std::vector<Value*> defaultValues;
	std::vector<std::string> captures;
	Block* body;
	int pos;
	std::map<std::string, SemanticVar*> vars;
	bool function_added;

	Function();
	virtual ~Function();

	void addArgument(Token* token, bool reference, Value* defaultValue);
	void can_return(Type type);

	virtual void print(std::ostream&, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	bool will_take(SemanticAnalyser*, const unsigned pos, const Type) override;

	void must_return(SemanticAnalyser*, const Type&) override;

	void analyse_body(SemanticAnalyser*, const Type& req_type);

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
