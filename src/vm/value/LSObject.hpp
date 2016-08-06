#ifndef LSOBJECT_HPP_
#define LSOBJECT_HPP_

#include "../LSValue.hpp"
#include "LSClass.hpp"
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSObject : public LSValue {

public:

	std::map<std::string, LSValue*> values;
	LSClass* clazz;
	bool readonly;

	static LSValue* object_class;

	LSObject();
	LSObject(std::initializer_list<std::pair<std::string, LSValue*>>);
	LSObject(LSClass*);
	LSObject(Json& data);

	virtual ~LSObject();

	void addField(std::string name, LSValue* value);
	LSArray<LSValue*>* get_keys() const;
	LSArray<LSValue*>* get_values() const;

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSValue* operator + (const LSValue*) const override;
	LSValue* operator += (LSValue*) override;
	LSValue* operator - (const LSValue*) const override;
	LSValue* operator -= (LSValue*) override;
	LSValue* operator * (const LSValue*) const override;
	LSValue* operator *= (LSValue*) override;
	LSValue* operator / (const LSValue*) const override;
	LSValue* operator /= (LSValue*) override;
	LSValue* poww(const LSValue*) const override;
	LSValue* pow_eq(LSValue*) override;
	LSValue* operator % (const LSValue*) const override;
	LSValue* operator %= (LSValue*) override;

	bool operator == (const LSValue*) const override;
	bool operator == (const LSObject*) const override;

	bool operator < (const LSValue*) const override;
	bool operator < (const LSObject*) const override;

	bool in(const LSValue*) const override;

	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* range(int start, int end) const override;
	LSValue* rangeL(int start, int end) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override;

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
