#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include <stack>
#include <vector>
#include <map>
#include "../../vm/VM.hpp"
#include "../error/Error.hpp"
#include "../semantic/Call.hpp"

namespace ls {

class Program;
class Module;
class Function;
class VariableValue;
class Context;
class Value;
class SemanticAnalyzer;
class Token;
class VariableDeclaration;
class Callable;
class Call;

class SemanticAnalyzer {
public:

	Program* program;
	VM* vm;

	std::vector<std::vector<std::map<std::string, std::shared_ptr<Variable>>>> variables;
	std::vector<std::map<std::string, std::shared_ptr<Variable>>> parameters;

	std::vector<Function*> functions;
	std::stack<FunctionVersion*> functions_stack;
	std::stack<int> loops;

	std::vector<Error> errors;

	SemanticAnalyzer();

	void analyze(Program*, Context*);

	void enter_function(FunctionVersion*);
	void leave_function();
	void enter_block();
	void leave_block();
	void add_function(Function*);
	FunctionVersion* current_function() const;

	void enter_loop();
	void leave_loop();
	bool in_loop(int deepness) const;

	std::shared_ptr<Variable> add_var(Token*, const Type*, Value*, VariableDeclaration*);
	std::shared_ptr<Variable> add_global_var(Token*, const Type*, Value*, VariableDeclaration*);
	std::shared_ptr<Variable> add_parameter(Token*, const Type*);

	std::shared_ptr<Variable> get_var(Token* name);
	std::map<std::string, std::shared_ptr<Variable>>& get_local_vars();
	std::shared_ptr<Variable> convert_var_to_any(std::shared_ptr<Variable> var);

	void add_error(Error ex);

};

}

#endif
