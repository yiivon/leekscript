#ifndef MAP_HPP
#define MAP_HPP

#include <vector>

#include "../../compiler/value/Expression.hpp"

namespace ls {

class Map : public Value {
public:
	std::vector<Value*> keys;
	std::vector<Value*> expressions;

	Map();
	virtual ~Map();

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const;
	virtual unsigned line() const;
	virtual bool will_take(SemanticAnalyser*, const unsigned, const Type);
	virtual bool will_take_element(SemanticAnalyser*, const Type);
	virtual bool must_be_pointer(SemanticAnalyser*);
	virtual void must_return(SemanticAnalyser*, const Type&);
	virtual void analyse(SemanticAnalyser*, const Type&);
	virtual jit_value_t compile(Compiler&) const;
};

}

#endif
