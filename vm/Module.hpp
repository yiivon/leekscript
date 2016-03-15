#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "Type.hpp"
#include "../parser/semantic/SemanticAnalyser.hpp"
#include "../parser/Program.hpp"

class ModuleMethod {
public:
	std::string name;
	Type type;
	void* addr;
	ModuleMethod(std::string name, Type type, void* addr)
	: name(name), type(type), addr(addr) {}
};

class ModuleAttribute {
public:
	std::string name;
	Type type;
	std::string value;
	ModuleAttribute(std::string name, Type type, std::string value)
	: name(name), type(type), value(value) {}
};

class Module {
public:

	std::string name;
	std::vector<ModuleAttribute> attributes;
	std::vector<ModuleMethod> methods;

	Module(std::string name);
	virtual ~Module();

	void method(std::string name, Type return_type, std::initializer_list<Type> args, void* addr);
	void attr(std::string name, Type type, std::string value);

	void include(SemanticAnalyser*, Program*);
	void generate_doc(std::ostream& os, JsonValue translation);
};

#endif
