#ifndef LSFUNCTION_H_
#define LSFUNCTION_H_

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include "../LSValue.hpp"
#include <map>
#include <jit/jit.h>
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSFunction : public LSValue {
public:

	static LSClass* function_class;

	bool native_function;
	void* function;
	std::map<std::string, LSValue*> values;

	LSFunction(void* function);
	LSFunction(void* function, int refs, bool native);
	LSFunction(Json& data);

	virtual ~LSFunction();

	bool native() const override;

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
	bool operator == (const LSFunction*) const override;

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
