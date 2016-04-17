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

class ModuleStaticField {
public:
	std::string name;
	Type type;
	std::string value;
	ModuleStaticField(std::string name, Type type, std::string value)
	: name(name), type(type), value(value) {}
};

class ModuleField {
public:
	std::string name;
	Type type;
	ModuleField(std::string name, Type type) : name(name), type(type) {}
};


class Module : public Type {
public:

	std::string name;
	LSClass* clazz;
	std::vector<ModuleField> fields;
	std::vector<ModuleMethod> methods;
	std::vector<ModuleStaticField> static_fields;
	std::vector<ModuleMethod> static_methods;

	Module(std::string name);
	virtual ~Module();

	void method(std::string name, Type return_type, std::initializer_list<Type> args, void* addr);
	void field(std::string name, Type type);
	void static_field(std::string name, Type type, std::string value);

	void include(SemanticAnalyser*, Program*);
	void generate_doc(std::ostream& os, std::string translation);
};

#endif
