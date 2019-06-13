#ifndef SEMANTIC_VAR_HPP
#define SEMANTIC_VAR_HPP

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

	void must_be_any(SemanticAnalyzer*);
};

}

#endif