#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "Expression.hpp"

class Object : public Value {
public:

	vector<Ident*> keys;
	vector<Value*> values;

	Object();
	virtual ~Object();

	virtual void print(ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
