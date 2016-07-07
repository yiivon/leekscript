#ifndef SEMANTICANALYSER_H_
#define SEMANTICANALYSER_H_

#include <stack>
#include <vector>
#include <map>

#include "../../vm/Type.hpp"

namespace ls {

class Program;
class Module;
class Function;
class VariableValue;
class Context;
class Value;
class SemanticAnalyser;
class Token;

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
	void will_take_element(SemanticAnalyser*, const Type&);
	void must_be_pointer(SemanticAnalyser*);
};

class SemanticAnalyser {
public:

	Program* program;
	bool in_block = false;
	bool in_function = false;
	bool in_program = false;
	bool reanalyse = false;

	std::map<std::string, SemanticVar*> internal_vars;
	std::vector<std::map<std::string, SemanticVar*>> variables;
	std::vector<std::map<std::string, SemanticVar*>> parameters;

	std::vector<Function*> functions;
	std::stack<Function*> functions_stack;

	SemanticAnalyser();
	virtual ~SemanticAnalyser();

	void analyse(Program*, Context*, std::vector<Module*>&);

	void enter_function(Function*);
	void leave_function();
	void enter_block();
	void leave_block();
	void add_function(Function*);
	Function* current_function() const;

	SemanticVar* add_var(Token*, Type, Value*);
	SemanticVar* add_parameter(Token*, Type);

	SemanticVar* get_var(Token* name);
	SemanticVar* get_var_direct(std::string name);
	std::map<std::string, SemanticVar*>& get_local_vars();

};

}

#endif
