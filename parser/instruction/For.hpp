#ifndef FOR_HPP
#define FOR_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "../value/Value.hpp"
#include "../Body.hpp"
#include "../value/Expression.hpp"
#include "../semantic/SemanticAnalyser.hpp"
class SemanticVar;
using namespace std;

class For : public Instruction {
public:

	vector<string> variables;
	vector<Value*> variablesValues;
	Value* condition;
	vector<Value*> iterations;
	Body* body;

	map<string, SemanticVar*> vars;

	For();
	virtual ~For();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
