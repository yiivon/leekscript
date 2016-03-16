#ifndef VALUE_HPP
#define VALUE_HPP

#include "../../vm/VM.hpp"
#include "../../Compiler.hpp"
class SemanticAnalyser;

class Value {
public:

	Type type;
	std::map<std::string, Type> attr_types;
	bool constant;
	bool parenthesis = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&) const = 0;

	void analyse(SemanticAnalyser*);
	virtual bool will_take(SemanticAnalyser*, const unsigned, const Type);
	virtual void analyse(SemanticAnalyser*, const Type) = 0;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const = 0;
};

#endif
