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
class StaticMethod;
class ModuleStaticField;

class LSClass : public LSValue {
public:

	class Operator {
	public:
		Type object_type;
		Type operand_type;
		Type return_type;
		void* addr;
		std::vector<TypeMutator*> mutators;
		bool native;
		bool v1_addr;
		bool v2_addr;

		Operator(Type object_type, Type operand, Type return_type, void* addr, std::initializer_list<TypeMutator*> mutators = {}, bool native = false, bool v1_addr = false, bool v2_addr = false)
		: object_type(object_type), operand_type(operand), return_type(return_type), addr(addr), mutators(mutators), native(native), v1_addr(v1_addr), v2_addr(v2_addr) {}
		static bool NATIVE;
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
	std::map<std::string, std::vector<StaticMethod>> static_methods;
	std::map<std::string, ModuleStaticField> static_fields;
	std::map<std::string, std::vector<Operator>> operators;

	static LSValue* clazz;

	LSClass(std::string);

	virtual ~LSClass();

	void addMethod(std::string&, std::vector<Method>);
	void addField(std::string, Type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void addField(std::string, Type, void* fun);
	void addStaticField(ModuleStaticField f);
	void addOperator(std::string name, std::vector<Operator>);

	Method* getMethod(std::string&, Type obj_type, std::vector<Type>&);
	void addStaticMethod(std::string& name, std::vector<StaticMethod> method);
	LSFunction* getDefaultMethod(const std::string& name);
	StaticMethod* getStaticMethod(std::string&, std::vector<Type>&);
	Operator* getOperator(std::string& name, Type& object_type, Type& operand_type);

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
