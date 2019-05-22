#ifndef LSCLASS_HPP_
#define LSCLASS_HPP_

#include <string>
#include <map>
#include "../LSValue.hpp"
#include "../../type/Type.hpp"
#include "../../compiler/Compiler.hpp"
#include "../TypeMutator.hpp"

namespace ls {

class Method;
class ModuleStaticField;

class LSClass : public LSValue {
public:

	class Operator {
	public:
		Type object_type;
		Type operand_type;
		Type return_type;
		void* addr = nullptr;
		std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func = nullptr;
		int flags;
		std::vector<TypeMutator*> mutators;
		bool v1_addr;
		bool v2_addr;
		std::vector<Type> templates;

		Operator(Type object_type, Type operand, Type return_type, void* addr, int flags = 0, std::initializer_list<TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false, std::vector<Type> templates = {})
			: object_type(object_type), operand_type(operand), return_type(return_type), addr(addr), flags(flags), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), templates(templates) {}
		Operator(Type object_type, Type operand, Type return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, bool)> func, int flags = 0, std::initializer_list<TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false, std::vector<Type> templates = {})
			: object_type(object_type), operand_type(operand), return_type(return_type), func(func), flags(flags), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), templates(templates) {}
	};

	class field {
	public:
		std::string name;
		Type type;
		std::function<Compiler::value(Compiler&, Compiler::value)> fun;
		void* native_fun;
		LSValue* default_value;
		field(std::string name, Type type) : name(name), type(type), fun(nullptr), default_value(nullptr) {}
		field(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun, LSValue* default_value) : name(name), type(type), fun(fun), default_value(default_value) {}
		field(std::string name, Type type, void* fun, LSValue* default_value) : name(name), type(type), native_fun(fun), default_value(default_value) {}
	};

	LSClass* parent;
	std::string name;

	std::map<std::string, field> fields;
	std::map<std::string, std::vector<Method>> methods;
	std::map<std::string, ModuleStaticField> static_fields;
	std::map<std::string, std::vector<Operator>> operators;

	static LSValue* clazz;
	static LSClass* constructor(char* name);

	LSClass(std::string);

	virtual ~LSClass();

	void addMethod(std::string, std::initializer_list<Method>, std::vector<Type> templates = {});
	void addField(std::string, Type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void addField(std::string, Type, void* fun);
	void addStaticField(ModuleStaticField f);
	void addOperator(std::string name, std::vector<Operator>);

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
