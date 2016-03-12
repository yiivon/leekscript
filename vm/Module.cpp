#include "Module.hpp"
#include "LSValue.hpp"

using namespace std;

Module::Module(string name) : name(name) {
	analyser = nullptr;
	program = nullptr;
	var = nullptr;
	clazz = nullptr;
}

Module::~Module() {}

void Module::init(SemanticAnalyser* analyser, Program* program) {

	clazz = new LSClass(name);
	program->system_vars.insert(pair<string, LSValue*>(name, clazz));
	var = analyser->add_var(name, Type::CLASS, nullptr);

	include();
}

void Module::method(string name, Type return_type, initializer_list<Type> args, void* addr) {

	Type type(RawType::FUNCTION, Nature::POINTER);
	type.setReturnType(return_type);

	for (Type arg : args) {
		type.addArgumentType(arg);
	}
	var->attr_types.insert(pair<string, Type>(name, type));
	clazz->addStaticField(name, new LSFunction(addr));
}
