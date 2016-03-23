#ifndef SEMANTICANALYSER_H_
#define SEMANTICANALYSER_H_

class Program;
#include <stack>
class Function;
#include "../../parser/value/VariableValue.hpp"

enum class VarScope {
	INTERNAL, GLOBAL, LOCAL, PARAMETER
};

class SemanticVar {
public:
	VarScope scope;
	Type type;
	std::map<std::string, Type> attr_types;
	int index;
	Value* value;
	SemanticVar(VarScope scope, Type type, int index, Value* value) :
		scope(scope), type(type), index(index), value(value) {}

	void will_take(SemanticAnalyser*, unsigned, const Type&);
};

class SemanticAnalyser {
public:

	Program* program;
	bool in_function = false;
	bool in_program = false;
	bool reanalyse = false;

	std::map<std::string, SemanticVar*> internal_vars;
	std::map<std::string, SemanticVar*> global_vars;
	std::vector<std::map<std::string, SemanticVar*>> local_vars;
	std::vector<std::map<std::string, SemanticVar*>> parameters;

	std::vector<Function*> functions;
	std::stack<Function*> functions_stack;

	SemanticAnalyser();
	virtual ~SemanticAnalyser();

	void analyse(Program*, Context*);

	void enter_function(Function*);
	void leave_function();
	void add_function(Function*);
	Function* current_function() const;

	SemanticVar* add_var(Token*, Type, Value*);
	SemanticVar* add_parameter(Token*, Type);
	void add_global_var(Token*, Type, Value*);

	SemanticVar* get_var(Token* name);
	SemanticVar* get_var_direct(std::string name);
	std::map<std::string, SemanticVar*>& get_local_vars();

};

#endif
