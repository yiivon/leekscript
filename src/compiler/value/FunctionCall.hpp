#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>

#include "../../compiler/value/Value.hpp"

namespace ls {

class FunctionCall : public Value {
public:

	Token* token;
	Value* function;
	std::vector<Value*> arguments;

	bool is_native = false;
	std::string native_func;
	Type return_type;
	void* std_func;
	Value* this_ptr;
	bool is_native_method = false;
	bool is_unknown_method = false;
	Value* object = nullptr;

	FunctionCall(Token* t);
	virtual ~FunctionCall();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	bool will_take(SemanticAnalyser*, const std::vector<Type>& args, int level);

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
