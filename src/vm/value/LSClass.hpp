#ifndef LSCLASS_HPP_
#define LSCLASS_HPP_

#include <string>
#include <map>
#include "../LSValue.hpp"
#include "../../type/Type.hpp"
#include "../../compiler/Compiler.hpp"
#include "../TypeMutator.hpp"
#include "../../compiler/semantic/Callable.hpp"

namespace ls {

class Method;
class ModuleStaticField;

class LSClass : public LSValue {
public:

	class field {
	public:
		std::string name;
		Type type;
		std::function<Compiler::value(Compiler&, Compiler::value)> fun = nullptr;
		std::function<Compiler::value(Compiler&)> static_fun = nullptr;
		void* native_fun = nullptr;
		void* addr = nullptr;
		LSValue* default_value = nullptr;
		LSValue* value = nullptr;
		field(std::string name, Type type) : name(name), type(type), fun(nullptr), default_value(nullptr) {}
		field(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun, LSValue* default_value) : name(name), type(type), fun(fun), default_value(default_value) {}
		field(std::string name, Type type, std::function<Compiler::value(Compiler&)> static_fun) : name(name), type(type), static_fun(static_fun) {}
		field(std::string name, Type type, void* fun, LSValue* default_value) : name(name), type(type), native_fun(fun), default_value(default_value) {}
		field(std::string name, Type type, void* addr, bool) : name(name), type(type), addr(addr) {}
		field(std::string name, Type type, LSValue* value) : name(name), type(type), value(value) {}
	};

	LSClass* parent;
	std::string name;
	std::map<std::string, field> fields;
	std::map<std::string, field> static_fields;
	std::map<std::string, Callable> methods;
	std::map<std::string, Callable> operators;

	static LSValue* clazz;
	static LSClass* constructor(char* name);

	LSClass(std::string);

	virtual ~LSClass();

	void addMethod(std::string, std::initializer_list<CallableVersion>, std::vector<Type> templates = {});
	void addField(std::string, Type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void addField(std::string, Type, void* fun);
	void addStaticField(field f);
	void addOperator(std::string name, std::initializer_list<CallableVersion>, std::vector<Type> templates = {});

	LSFunction* getDefaultMethod(const std::string& name);
	const Callable* getOperator(SemanticAnalyzer* analyzer, std::string& name, Type& object_type, Type& operand_type);

	bool to_bool() const override;
	virtual bool ls_not() const override;

	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	LSValue* attr(const std::string& key) const override;

	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;

	LSValue* getClass() const override;
};

}

#endif
