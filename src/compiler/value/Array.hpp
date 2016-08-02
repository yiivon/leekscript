#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <vector>

#include "../../compiler/value/Expression.hpp"

namespace ls {

class Array : public Value {
public:

	bool associative = false;
	std::vector<Value*> keys;
	std::vector<Value*> expressions;
	bool interval = false;
	Type supported_type;

	Array();
	virtual ~Array();

	void addValue(Value* value, Value* key);

	virtual void print(std::ostream&, bool debug) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	void elements_will_take(SemanticAnalyser*, const unsigned, const Type&, int level);

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
