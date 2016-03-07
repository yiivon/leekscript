#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "../value/Value.hpp"
#include "../Body.hpp"
#include "../value/Expression.hpp"
#include "../semantic/SemanticAnalyser.hpp"
using namespace std;

class Foreach : public Instruction {
public:

	string key = "";
	string value = "";
	Value* array;
	Body* body;
	Type var_type;

	Foreach();
	virtual ~Foreach();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
