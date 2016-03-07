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
	map<string, Type> attr_types;
	int index;
	Value* value;
	bool internal;
	SemanticVar(VarScope scope, Type type, int index, Value* value, bool internal) :
		scope(scope), type(type), index(index), value(value), internal(internal) {}

	void will_take(SemanticAnalyser*, unsigned, const Type&);
};

class SemanticAnalyser {
public:

	Program* program;
	bool in_function = false;
	bool in_program = false;
	bool reanalyse = false;

	map<string, SemanticVar*> internal_vars;
	map<string, SemanticVar*> global_vars;
	vector<map<string, SemanticVar*>> local_vars;
	vector<map<string, SemanticVar*>> parameters;

	vector<Function*> functions;
	stack<Function*> functions_stack;

	SemanticAnalyser();
	virtual ~SemanticAnalyser();

	void analyse(Program*, Context*);

	void enter_function(Function*);
	void leave_function();
	void add_function(Function*);
	Function* current_function() const;

	SemanticVar* add_var(string, Type, Value*);
	SemanticVar* add_var(string, Type, Value*, bool internal);
	SemanticVar* add_parameter(string name, Type);
	void add_global_var(string, Type, Value*);

	SemanticVar* get_var(Token* name);
	SemanticVar* get_var_direct(string name);
	map<string, SemanticVar*>& get_local_vars();

};

#endif
