#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "Type.hpp"
#include "../parser/semantic/SemanticAnalyser.hpp"
#include "../parser/Program.hpp"

namespace ls {

class LSClass;

class StaticMethod {
public:
	Type type;
	void* addr;
	StaticMethod(Type return_type, std::initializer_list<Type> args, void* addr) {
		this->addr = addr;
		type = {RawType::FUNCTION, Nature::POINTER};
		type.setReturnType(return_type);
		for (Type arg : args) {
			type.addArgumentType(arg);
		}
	}
};

class Method {
public:
	Type type;
	void* addr;
	Type obj_type;
	Method(Type obj_type, Type return_type, std::initializer_list<Type> args, void* addr) {
		this->addr = addr;
		this->obj_type = obj_type;
		type = {RawType::FUNCTION, Nature::POINTER};
		type.setReturnType(return_type);
		for (Type arg : args) {
			type.addArgumentType(arg);
		}
	}
};

class ModuleMethod {
public:
	std::string name;
	std::vector<Method> impl;
	ModuleMethod(std::string name, std::vector<Method> impl)
	: name(name), impl(impl) {}
};

class ModuleStaticMethod {
public:
	std::string name;
	std::vector<StaticMethod> impl;
	ModuleStaticMethod(std::string name, std::vector<StaticMethod> impl)
	: name(name), impl(impl) {}
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
	std::vector<ModuleStaticMethod> static_methods;

	Module(std::string name);
	virtual ~Module();

	void method(std::string name, std::initializer_list<Method>);
	void method(std::string name, Type obj_type, Type return_type, std::initializer_list<Type> args, void* addr);

	void static_method(std::string name, std::initializer_list<StaticMethod>);
	void static_method(std::string name, Type return_type, std::initializer_list<Type> args, void* addr);

	void field(std::string name, Type type);
	void static_field(std::string name, Type type, std::string value);

	void include(SemanticAnalyser*, Program*);
	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
