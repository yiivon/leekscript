#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"
#include "Function.hpp"

namespace ls {

class Callable;
class CallableVersion;

class FunctionCall : public Value {
public:
	std::shared_ptr<Token> token;
	Value* function;
	std::vector<Value*> arguments;
	std::shared_ptr<Token> closing_parenthesis;
	bool is_native = false;
	Type return_type;
	void* std_func;
	Value* this_ptr;
	bool is_native_method = false;
	bool is_unknown_method = false;
	Value* object = nullptr;
	Function* function_object;
	Type function_type;
	std::vector<Type> arg_types;
	std::string function_name;
	const CallableVersion* callable_version;

	FunctionCall(std::shared_ptr<Token> t);
	virtual ~FunctionCall();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual Call* get_callable(SemanticAnalyzer*, int argument_count) const override;
	virtual void analyze(SemanticAnalyzer*) override;
	bool will_take(SemanticAnalyzer*, const std::vector<Type>& args, int level);
	void set_version(const std::vector<Type>& args, int level) override;
	virtual Type version_type(std::vector<Type>) const override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
