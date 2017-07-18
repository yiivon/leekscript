#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "Type.hpp"
#include "value/LSClass.hpp"
#include "../compiler/Compiler.hpp"

namespace ls {

class LSValue;

class StaticMethod {
public:
	Type type;
	void* addr;
	bool native;
	StaticMethod(Type return_type, std::vector<Type> args, void* addr, bool native = false) {
		this->addr = addr;
		type = {RawType::FUNCTION, Nature::POINTER};
		type.setReturnType(return_type);
		for (auto& arg : args) {
			type.addArgumentType(arg);
		}
		this->native = native;
	}
};

class Method : public StaticMethod {
public:
	Type obj_type;

	Method(Type obj_type, Type return_type, std::vector<Type> args, void* addr, bool native = false)
		: StaticMethod(return_type, args, addr, native), obj_type(obj_type) {}

	enum Option {
		Static, Instantiate, Both
	};

	static bool NATIVE;
};

class MethodConstructor {
public:
	Type return_type;
	void* addr;
	bool native;
	Type obj_type;
	std::vector<Type> args;

	MethodConstructor(Type return_type, std::initializer_list<Type> args, void* addr, bool native = false)
		: return_type(return_type), addr(addr), native(native), args(args) {}
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
	std::function<Compiler::value(Compiler&)> fun = nullptr;
	void* native_fun = nullptr;
	LSValue* value = nullptr;

	ModuleStaticField(const ModuleStaticField& f)
	: name(f.name), type(f.type), fun(f.fun), native_fun(f.native_fun), value(f.value) {}
	ModuleStaticField(std::string name, Type type, LSValue* value)
	: name(name), type(type), value(value) {}
	ModuleStaticField(std::string name, Type type, std::function<Compiler::value(Compiler&)> fun)
	: name(name), type(type), fun(fun) {}
	ModuleStaticField(std::string name, Type type, void* fun)
	: name(name), type(type), native_fun(fun) {}
};

class Module {
public:

	std::string name;
	LSClass* clazz;

	Module(std::string name);
	virtual ~Module();

	void operator_(std::string name, std::initializer_list<LSClass::Operator>);

	void method(std::string name, Method::Option opt, std::initializer_list<MethodConstructor> methods);
	void method(std::string name, std::initializer_list<MethodConstructor> methods) { method(name, Method::Both, methods); }

	void field(std::string name, Type type);
	void field(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void field(std::string name, Type type, void* fun);
	void static_field(std::string name, Type type, std::function<Compiler::value(Compiler&)> fun);
	void static_field(std::string name, Type type, void* fun);

	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
