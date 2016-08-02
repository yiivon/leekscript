#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "../../compiler/value/Expression.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class Object : public Value {
public:

	std::vector<Ident*> keys;
	std::vector<Value*> values;

	Object();
	virtual ~Object();

	virtual void print(std::ostream&, bool debug) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
