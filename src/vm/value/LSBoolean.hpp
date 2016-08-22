#ifndef LSBOOLEAN_HPP_
#define LSBOOLEAN_HPP_

#include <string>
#include "../../../lib/json.hpp"
#include "../LSValue.hpp"
#include "../Type.hpp"

namespace ls {

class LSBoolean : public LSValue {
private:

	LSBoolean();
	LSBoolean(bool value);

public:

	const bool value;

	static LSValue* boolean_class;
	static LSBoolean* false_val;
	static LSBoolean* true_val;
	static LSBoolean* get(bool);

	LSBoolean(Json& data);

	virtual ~LSBoolean();

	bool native() const override;

	bool isTrue() const override;

	LSValue* operator ! () const override;
	LSValue* operator ~ () const override;

	LSValue* ls_radd(LSValue* value) override    { return value->ls_add(this); }
	LSValue* ls_radd_eq(LSValue* value) override { return value->ls_add_eq(this); }
	LSValue* ls_rsub(LSValue* value) override    { return value->ls_sub(this); }

	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_sub(LSNumber*) override;
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
	bool operator == (const LSBoolean*) const override;

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
