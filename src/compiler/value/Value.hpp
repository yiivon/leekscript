#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>

#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../../vm/Type.hpp"

namespace ls {

class SemanticAnalyser;

class Value {
public:

	Type type;
	std::map<std::string, Type> attr_types;
	bool constant;
	bool parenthesis = false;
	bool can_return = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&) const = 0;
	virtual unsigned line() const = 0;

	virtual bool will_take(SemanticAnalyser*, const unsigned, const Type);
	virtual bool will_take_element(SemanticAnalyser*, const Type);
	virtual bool must_be_pointer(SemanticAnalyser*);
	virtual void must_return(SemanticAnalyser*, const Type&);
	void analyse(SemanticAnalyser*);
	virtual void analyse(SemanticAnalyser*, const Type) = 0;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const = 0;
};

}

#endif
