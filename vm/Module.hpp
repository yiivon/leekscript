#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "Type.hpp"
#include "../parser/semantic/SemanticAnalyser.hpp"
#include "../parser/Program.hpp"

class Module {
public:

	std::string name;
	SemanticAnalyser* analyser;
	Program* program;
	LSClass* clazz;
	SemanticVar* var;

	Module(std::string name);
	virtual ~Module();

	void init(SemanticAnalyser*, Program*);
	virtual void include() = 0;
	void method(std::string name, Type return_type, std::initializer_list<Type> args, void* addr);
};

#endif
