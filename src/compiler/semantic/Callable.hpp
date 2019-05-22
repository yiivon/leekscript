#ifndef CALLABLE_HPP_
#define CALLABLE_HPP_

#include "../../type/Type.hpp"
#include "../value/Value.hpp"
#include "../../vm/TypeMutator.hpp"
#include "../value/Function.hpp"
#include "../../vm/Module.hpp"

namespace ls {

class CallableVersion {
public:
	std::string name;
	Type type;
	Value* object = nullptr;
	bool symbol = false;
	const Value* value = nullptr;
	std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func = nullptr;
	Function::Version* user_fun = nullptr;
	std::vector<TypeMutator*> mutators;
	std::vector<Type> templates;
	bool unknown = false;
	bool v1_addr = false;
	bool v2_addr = false;
	int flags = 0;

	CallableVersion(std::string name, Type type, std::vector<TypeMutator*> mutators = {}, std::vector<Type> templates = {}, Value* object = nullptr, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0)
		: name(name), type(type), object(object), symbol(true), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {
			assert(name.find(".") != std::string::npos);
		}
	CallableVersion(std::string name, Type type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func, std::vector<TypeMutator*> mutators = {}, std::vector<Type> templates = {}, Value* object = nullptr, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0)
		: name(name), type(type), object(object), func(func), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}
	CallableVersion(std::string name, Type type, const Value* value, std::vector<TypeMutator*> mutators = {}, std::vector<Type> templates = {}, Value* object = nullptr, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0)
		: name(name), type(type), object(object), value(value), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}
	CallableVersion(std::string name, Type type, Function::Version* f, std::vector<TypeMutator*> mutators = {}, std::vector<Type> templates = {}, Value* object = nullptr, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0)
		: name(name), type(type), object(object), user_fun(f), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}

	void apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> arguments);
	void resolve_templates(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const;

	Compiler::value compile_call(Compiler& c, std::vector<Compiler::value> args, bool no_return) const;
};

class Callable {
public:
	std::string name;
	std::vector<CallableVersion> versions;

	Callable(std::string name) : name(name) {}
	Callable(std::string name, std::initializer_list<CallableVersion> versions) : name(name), versions(versions) {}
	Callable(std::initializer_list<CallableVersion> versions) : name("?"), versions(versions) {}
	void add_version(CallableVersion v);
	CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const;
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::CallableVersion&);
	std::ostream& operator << (std::ostream&, const ls::CallableVersion*);
	std::ostream& operator << (std::ostream&, const ls::Callable*);
}

#endif