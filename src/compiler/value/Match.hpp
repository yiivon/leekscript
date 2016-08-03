#ifndef MATCH_HPP_
#define MATCH_HPP_

#include "../../compiler/value/Value.hpp"
#include <vector>

namespace ls {

class Match : public Value
{
public:
	Value* value;
	std::vector<Value*> patterns;
	std::vector<Value*> returns;

	Match();
	virtual ~Match();

	virtual void print(std::ostream&, bool debug = false) const override;
	virtual unsigned line() const override;
	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
