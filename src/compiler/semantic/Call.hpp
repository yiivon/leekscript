#ifndef CALL_HPP_
#define CALL_HPP_

#include "../../type/Type.hpp"
#include <vector>
#include "../Compiler.hpp"

namespace ls {

class SemanticAnalyzer;
class CallableVersion;
class Callable;

class Call {
public:
	Callable* callable = nullptr;
	Value* object = nullptr;
	Compiler::value compiled_object;

	Call();
	Call(Callable* callable) : callable(callable) {}
	Call(std::vector<const CallableVersion*> versions);
	Call(std::initializer_list<const CallableVersion*> versions);
	void add_version(const CallableVersion* v);

	const CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const;
	void apply_mutators(SemanticAnalyzer* analyzer, const CallableVersion* version, std::vector<Value*> arguments) const;
	void pre_compile_call(Compiler& c) const;
	Compiler::value compile_call(Compiler& c, const CallableVersion* version, std::vector<Compiler::value> args, bool no_return) const;
	
};

}

#endif