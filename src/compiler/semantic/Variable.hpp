#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "Call.hpp"

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

class Variable {
public:
	std::string name;
	VarScope scope;
	int index;
	int parent_index;
	Value* value;
	FunctionVersion* function; // In which function the variable is declared
	const Type* type;
	std::vector<const Type*> version;
	LSValue* lsvalue = nullptr;
	Call call;

	Variable(std::string name, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, LSValue* lsvalue, Call call = {}) :
		name(name), scope(scope), index(index), parent_index(0), value(value), function(function), type(type), lsvalue(lsvalue), call(call) {}
};

}

#endif