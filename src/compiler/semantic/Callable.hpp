#ifndef CALLABLE_HPP_
#define CALLABLE_HPP_

#include "../../type/Type.hpp"
#include "../value/Value.hpp"
#include "../../vm/TypeMutator.hpp"

namespace ls {

class CallableVersion {
public:
	std::string name;
	Type type;
	Value* object = nullptr;
	void* addr = nullptr;
	const Value* value = nullptr;
	std::function<Compiler::value(Compiler&, std::vector<Compiler::value>)> func = nullptr;
	std::vector<TypeMutator*> mutators;
	std::vector<Type> templates;

	CallableVersion(std::string name, Type type, void* addr, std::vector<TypeMutator*> mutators, std::vector<Type> templates, Value* object)
		: name(name), type(type), object(object), addr(addr), mutators(mutators), templates(templates) {}
	CallableVersion(std::string name, Type type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>)> func, std::vector<TypeMutator*> mutators, std::vector<Type> templates, Value* object)
		: name(name), type(type), object(object), func(func), mutators(mutators), templates(templates) {}
	CallableVersion(std::string name, Type type, const Value* value, std::vector<TypeMutator*> mutators, std::vector<Type> templates, Value* object)
		: name(name), type(type), object(object), value(value), mutators(mutators), templates(templates) {}

	Compiler::value compile_call(Compiler& c, std::vector<Compiler::value> args) const;
};

class Callable {
public:
	std::string name;
	std::vector<CallableVersion> versions;

	Callable(std::string name) : name(name) {}
	void add_version(CallableVersion v);
	CallableVersion* resolve(SemanticAnalyser* analyser, std::vector<Type> arguments);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::CallableVersion&);
	std::ostream& operator << (std::ostream&, const ls::CallableVersion*);
	std::ostream& operator << (std::ostream&, const ls::Callable*);
}

#endif