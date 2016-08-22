#ifndef LSCLASS_HPP_
#define LSCLASS_HPP_

#include "../LSValue.hpp"
#include <string>
#include <map>
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class Method;
class StaticMethod;
class ModuleStaticField;

class LSClass : public LSValue {
public:

	LSClass* parent;
	std::string name;

	std::map<std::string, Type> fields;
	std::map<std::string, std::vector<Method>> methods;
	std::map<std::string, std::vector<StaticMethod>> static_methods;
	std::map<std::string, ModuleStaticField> static_fields;

	static LSValue* class_class;

	LSClass();
	LSClass(std::string);
	LSClass(std::string, int refs);
	LSClass(Json&);

	virtual ~LSClass();

	void addMethod(std::string&, std::vector<Method>&);
	void addField(std::string, Type);
	void addStaticField(ModuleStaticField& f);
	void addStaticField(std::string, Type type, LSValue*);
	Method* getMethod(std::string&, Type obj_type, std::vector<Type>&);
	void addStaticMethod(std::string& name, std::vector<StaticMethod>& method);
	LSFunction* getDefaultMethod(std::string& name);
	StaticMethod* getStaticMethod(std::string&, std::vector<Type>&);

	bool native() const override;

	bool isTrue() const override;

	LSVALUE_OPERATORS

	bool operator == (const LSValue*) const override;
	bool operator == (const LSClass*) const override;

	bool operator < (const LSValue*) const override;
	bool operator < (const LSNull*) const override;
	bool operator < (const LSBoolean*) const override;
	bool operator < (const LSNumber*) const override;
	bool operator < (const LSString*) const override;
	bool operator < (const LSArray<LSValue*>*) const override;
	bool operator < (const LSArray<int>*) const override;
	bool operator < (const LSArray<double>*) const override;
	bool operator < (const LSFunction*) const override;
	bool operator < (const LSObject*) const override;
	bool operator < (const LSClass*) const override;


	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* clone() const;

	std::ostream& print(std::ostream& os) const;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override;

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
