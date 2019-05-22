#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "../type/Type.hpp"
#include "value/LSClass.hpp"
#include "../compiler/Compiler.hpp"
#include "TypeMutator.hpp"

namespace ls {

class LSValue;

class Method {
public:
	Type type;
	void* addr;
	std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func;
	std::vector<TypeMutator*> mutators;
	std::vector<Type> templates;
	int flags;
	Method(Type return_type, std::vector<Type> args, void* addr, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func, std::vector<TypeMutator*> mutators = {}, std::vector<Type> templates = {}, int flags = 0) {
		this->addr = addr;
		type = Type::fun(return_type, args);
		this->func = func;
		this->mutators = mutators;
		this->templates = templates;
		this->flags = flags;
	}
};

class MethodConstructor {
public:
	Type return_type;
	void* addr = nullptr;
	std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func = nullptr;
	std::vector<Type> args;
	int flags;
	std::vector<TypeMutator*> mutators;
	MethodConstructor(Type return_type, std::initializer_list<Type> args, void* addr, int flags = 0, std::initializer_list<TypeMutator*> mutators = {})
		: return_type(return_type), addr(addr), args(args), flags(flags), mutators(mutators) {}
	MethodConstructor(Type return_type, std::initializer_list<Type> args, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func, int flags = 0, std::initializer_list<TypeMutator*> mutators = {})
		: return_type(return_type), func(func), args(args), flags(flags), mutators(mutators) {}
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

class Module;

class Template {
public:
	Module* module;
	std::vector<Type> templates;
	template<class... Args>
	Template(Module* module, Args... templates) : module(module), templates({templates...}) {}

	void operator_(std::string name, std::initializer_list<LSClass::Operator>);

	void method(std::string name, std::initializer_list<MethodConstructor> methods);
};

class Module {
public:
	static int THROWS;
	static int LEGACY;

	std::string name;
	LSClass* clazz;

	Module(std::string name);
	virtual ~Module();

	void operator_(std::string name, std::initializer_list<LSClass::Operator>, std::vector<Type> templates = {});

	template<class... Args>
	Template template_(Args... templates) {
		return { this, templates... };
	}

	void constructor_(std::initializer_list<MethodConstructor> methods);

	void method(std::string name, std::initializer_list<MethodConstructor> methods, std::vector<Type> templates = {});

	void field(std::string name, Type type);
	void field(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void field(std::string name, Type type, void* fun);
	void static_field(std::string name, Type type, std::function<Compiler::value(Compiler&)> fun);
	void static_field(std::string name, Type type, void* fun);

	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
