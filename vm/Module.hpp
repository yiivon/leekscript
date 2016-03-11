#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "Type.hpp"
#include "../parser/semantic/SemanticAnalyser.hpp"
#include "../parser/Program.hpp"

class Module {
public:

	string name;
	SemanticAnalyser* analyser;
	Program* program;
	LSClass* clazz;
	SemanticVar* var;

	Module(string name);
	virtual ~Module();

	void init(SemanticAnalyser*, Program*);
	virtual void include() = 0;
	void method(string name, Type return_type, initializer_list<Type> args, void* addr);
};

#endif
