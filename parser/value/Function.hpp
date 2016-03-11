#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>
using namespace std;

#include "Value.hpp"
#include "../lexical/Ident.hpp"
#include "../Body.hpp"
#include "../semantic/SemanticAnalyser.hpp"
class SemanticVar;

class Function : public Value {
public:

	bool lambda = false;
	vector<string> arguments;
	vector<bool> references;
	vector<Value*> defaultValues;
	vector<string> captures;
	Body* body;
	int pos;
	map<string, SemanticVar*> vars;
	bool function_added;

	Function();
	virtual ~Function();

	void addArgument(string, bool reference, Value* defaultValue);

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	bool will_take(SemanticAnalyser*, const unsigned pos, const Type) override;

	void analyse_body(SemanticAnalyser*, const Type& req_type);

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
