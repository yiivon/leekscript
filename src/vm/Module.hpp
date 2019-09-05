#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "value/LSClass.hpp"
#include "../compiler/Compiler.hpp"
#include "TypeMutator.hpp"
#include "../compiler/semantic/CallableVersion.hpp"

namespace ls {

class LSValue;
class CallableVersion;
class Module;
class VM;
class Type;

class Template {
public:
	Module* module;
	std::vector<const Type*> templates;
	template<class... Args>
	Template(Module* module, Args... templates) : module(module), templates({templates...}) {}

	void operator_(std::string name, std::initializer_list<CallableVersion>);
	void method(std::string name, std::initializer_list<CallableVersion> methods);
};

class Module {
public:
	static int THROWS;
	static int LEGACY;
	static int DEFAULT;
	static int EMPTY_VARIABLE;
	static int NO_RETURN;

	static bool STORE_ARRAY_SIZE;

	VM* vm;
	std::string name;
	std::unique_ptr<LSClass> clazz;

	Module(VM* vm, std::string name);
	virtual ~Module() {}

	void operator_(std::string name, std::initializer_list<CallableVersion>, std::vector<const Type*> templates = {});

	template<class... Args>
	Template template_(Args... templates) {
		return { this, templates... };
	}

	void constructor_(std::initializer_list<CallableVersion> methods);

	void method(std::string name, std::initializer_list<CallableVersion> methods, std::vector<const Type*> templates = {}, bool legacy = false);

	void field(std::string name, const Type* type);
	void field(std::string name, const Type* type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void field(std::string name, const Type* type, void* fun);
	void static_field(std::string name, const Type* type, std::function<Compiler::value(Compiler&)> fun);
	void static_field(std::string name, const Type* type, void* addr);
	void static_field_fun(std::string name, const Type* type, void* fun);

	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
