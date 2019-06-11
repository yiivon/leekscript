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

enum class VarScope {
	INTERNAL, LOCAL, PARAMETER, CAPTURE
};

class SemanticVar {
public:
	std::string name;
	VarScope scope;
	std::map<std::string, const Type*> attr_types;
	int index;
	int parent_index;
	Value* value;
	VariableDeclaration* vd;
	FunctionVersion* function; // In which function the variable is declared
	const Type* type;
	std::vector<const Type*> version;
	bool has_version = false;
	LSValue* lsvalue = nullptr;
	Call call;

	SemanticVar(std::string name, VarScope scope, const Type* type, int index, Value* value, VariableDeclaration* vd, FunctionVersion* function, LSValue* lsvalue, Call call = {}) :
		name(name), scope(scope), index(index), parent_index(0), value(value), vd(vd), function(function), type(type), lsvalue(lsvalue), call(call) {}

	// TODO remove ?
	SemanticVar(const SemanticVar& o) : name(o.name), scope(o.scope), attr_types(o.attr_types), index(o.index), parent_index(o.parent_index), value(o.value), vd(o.vd), function(o.function), type(o.type), lsvalue(o.lsvalue), call(o.call) {}

	void must_be_any(SemanticAnalyzer*);
};

class SemanticAnalyzer {
public:

	Program* program;
	VM* vm;

	std::vector<std::vector<std::map<std::string, std::shared_ptr<SemanticVar>>>> variables;
	std::vector<std::map<std::string, std::shared_ptr<SemanticVar>>> parameters;

	std::vector<Function*> functions;
	std::stack<FunctionVersion*> functions_stack;
	std::stack<int> loops;

	std::vector<Error> errors;

	SemanticAnalyzer();
	virtual ~SemanticAnalyzer();

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

	std::shared_ptr<SemanticVar> add_var(Token*, const Type*, Value*, VariableDeclaration*);
	std::shared_ptr<SemanticVar> add_parameter(Token*, const Type*);

	std::shared_ptr<SemanticVar> get_var(Token* name);
	std::map<std::string, std::shared_ptr<SemanticVar>>& get_local_vars();
	std::shared_ptr<SemanticVar> convert_var_to_any(std::shared_ptr<SemanticVar> var);

	void add_error(Error ex);

};

}

#endif
