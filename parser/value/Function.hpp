#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>

#include "Value.hpp"
#include "../lexical/Ident.hpp"
#include "../Body.hpp"
#include "../semantic/SemanticAnalyser.hpp"
class SemanticVar;

class Function : public Value {
public:

	bool lambda = false;
	std::vector<std::string> arguments;
	std::vector<bool> references;
	std::vector<Value*> defaultValues;
	std::vector<std::string> captures;
	Body* body;
	int pos;
	std::map<std::string, SemanticVar*> vars;
	bool function_added;

	Function();
	virtual ~Function();

	void addArgument(std::string, bool reference, Value* defaultValue);

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	bool will_take(SemanticAnalyser*, const unsigned pos, const Type) override;

	void analyse_body(SemanticAnalyser*, const Type& req_type);

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
