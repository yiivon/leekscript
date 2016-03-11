#ifndef LSBOOLEAN_HPP_
#define LSBOOLEAN_HPP_

#include <string>
#include "../../lib/gason.h"
#include "../LSValue.hpp"
#include "../Type.hpp"

class LSBoolean : public LSValue {
public:

	const bool value;

	static LSValue* boolean_class;
	static LSBoolean* false_val;
	static LSBoolean* true_val;
	static LSBoolean* get(bool);

	LSBoolean();
	LSBoolean(bool value);
	LSBoolean(JsonValue& data);

	virtual ~LSBoolean();

	bool isTrue() const override;

	LSValue* operator - () const override;
	LSValue* operator ! () const override;
	LSValue* operator ~ () const override;

	LSValue* operator ++ () override;
	LSValue* operator ++ (int) override;

	LSValue* operator -- () override;
	LSValue* operator -- (int) override;

	LSValue* operator + (const LSValue*) const override;
	LSValue* operator + (const LSNull*) const override;
	LSValue* operator + (const LSBoolean*) const override;
	LSValue* operator + (const LSNumber*) const override;
	LSValue* operator + (const LSString*) const override;
	LSValue* operator + (const LSArray*) const override;
	LSValue* operator + (const LSObject*) const override;
	LSValue* operator + (const LSFunction*) const override;
	LSValue* operator + (const LSClass*) const override;

	LSValue* operator += (LSValue*) const override;
	LSValue* operator += (const LSNull*) override;
	LSValue* operator += (const LSBoolean*) override;
	LSValue* operator += (const LSNumber*) override;
	LSValue* operator += (const LSString*) override;
	LSValue* operator += (const LSArray*) override;
	LSValue* operator += (const LSObject*) override;
	LSValue* operator += (const LSFunction*) override;
	LSValue* operator += (const LSClass*) override;

	LSValue* operator - (const LSValue*) const override;
	LSValue* operator - (const LSNull*) const override;
	LSValue* operator - (const LSBoolean*) const override;
	LSValue* operator - (const LSNumber*) const override;
	LSValue* operator - (const LSString*) const override;
	LSValue* operator - (const LSArray*) const override;
	LSValue* operator - (const LSObject*) const override;
	LSValue* operator - (const LSFunction*) const override;
	LSValue* operator - (const LSClass*) const override;

	LSValue* operator -= (LSValue*) const override;
	LSValue* operator -= (const LSNull*) override;
	LSValue* operator -= (const LSBoolean*) override;
	LSValue* operator -= (const LSNumber*) override;
	LSValue* operator -= (const LSString*) override;
	LSValue* operator -= (const LSArray*) override;
	LSValue* operator -= (const LSObject*) override;
	LSValue* operator -= (const LSFunction*) override;
	LSValue* operator -= (const LSClass*) override;

	LSValue* operator * (const LSValue*) const override;
	LSValue* operator * (const LSNull*) const override;
	LSValue* operator * (const LSBoolean*) const override;
	LSValue* operator * (const LSNumber*) const override;
	LSValue* operator * (const LSString*) const override;
	LSValue* operator * (const LSArray*) const override;
	LSValue* operator * (const LSObject*) const override;
	LSValue* operator * (const LSFunction*) const override;
	LSValue* operator * (const LSClass*) const override;

	LSValue* operator *= (LSValue*) const override;
	LSValue* operator *= (const LSNull*) override;
	LSValue* operator *= (const LSBoolean*) override;
	LSValue* operator *= (const LSNumber*) override;
	LSValue* operator *= (const LSString*) override;
	LSValue* operator *= (const LSArray*) override;
	LSValue* operator *= (const LSObject*) override;
	LSValue* operator *= (const LSFunction*) override;
	LSValue* operator *= (const LSClass*) override;

	LSValue* operator / (const LSValue*) const override;
	LSValue* operator / (const LSNull*) const override;
	LSValue* operator / (const LSBoolean*) const override;
	LSValue* operator / (const LSNumber*) const override;
	LSValue* operator / (const LSString*) const override;
	LSValue* operator / (const LSArray*) const override;
	LSValue* operator / (const LSObject*) const override;
	LSValue* operator / (const LSFunction*) const override;
	LSValue* operator / (const LSClass*) const override;

	LSValue* operator /= (LSValue*) const override;
	LSValue* operator /= (const LSNull*) override;
	LSValue* operator /= (const LSBoolean*) override;
	LSValue* operator /= (const LSNumber*) override;
	LSValue* operator /= (const LSString*) override;
	LSValue* operator /= (const LSArray*) override;
	LSValue* operator /= (const LSObject*) override;
	LSValue* operator /= (const LSFunction*) override;
	LSValue* operator /= (const LSClass*) override;

	LSValue* poww(const LSValue*) const override;
	LSValue* poww(const LSNull*) const override;
	LSValue* poww(const LSBoolean*) const override;
	LSValue* poww(const LSNumber*) const override;
	LSValue* poww(const LSString*) const override;
	LSValue* poww(const LSArray*) const override;
	LSValue* poww(const LSObject*) const override;
	LSValue* poww(const LSFunction*) const override;
	LSValue* poww(const LSClass*) const override;

	LSValue* pow_eq(LSValue*) const override;
	LSValue* pow_eq(const LSNull*) override;
	LSValue* pow_eq(const LSBoolean*) override;
	LSValue* pow_eq(const LSNumber*) override;
	LSValue* pow_eq(const LSString*) override;
	LSValue* pow_eq(const LSArray*) override;
	LSValue* pow_eq(const LSObject*) override;
	LSValue* pow_eq(const LSFunction*) override;
	LSValue* pow_eq(const LSClass*) override;

	LSValue* operator % (const LSValue*) const override;
	LSValue* operator % (const LSNull*) const override;
	LSValue* operator % (const LSBoolean*) const override;
	LSValue* operator % (const LSNumber*) const override;
	LSValue* operator % (const LSString*) const override;
	LSValue* operator % (const LSArray*) const override;
	LSValue* operator % (const LSObject*) const override;
	LSValue* operator % (const LSFunction*) const override;
	LSValue* operator % (const LSClass*) const override;

	LSValue* operator %= (LSValue*) const override;
	LSValue* operator %= (const LSNull*) override;
	LSValue* operator %= (const LSBoolean*) override;
	LSValue* operator %= (const LSNumber*) override;
	LSValue* operator %= (const LSString*) override;
	LSValue* operator %= (const LSArray*) override;
	LSValue* operator %= (const LSObject*) override;
	LSValue* operator %= (const LSFunction*) override;
	LSValue* operator %= (const LSClass*) override;

	bool operator == (const LSValue*) const override;
	bool operator == (const LSNull*) const override;
	bool operator == (const LSBoolean*) const override;
	bool operator == (const LSNumber*) const override;
	bool operator == (const LSString*) const override;
	bool operator == (const LSArray*) const override;
	bool operator == (const LSFunction*) const override;
	bool operator == (const LSObject*) const override;
	bool operator == (const LSClass*) const override;

	bool operator < (const LSValue*) const override;
	bool operator < (const LSNull*) const override;
	bool operator < (const LSBoolean*) const override;
	bool operator < (const LSNumber*) const override;
	bool operator < (const LSString*) const override;
	bool operator < (const LSArray*) const override;
	bool operator < (const LSFunction*) const override;
	bool operator < (const LSObject*) const override;
	bool operator < (const LSClass*) const override;

	bool operator > (const LSValue*) const override;
	bool operator > (const LSNull*) const override;
	bool operator > (const LSBoolean*) const override;
	bool operator > (const LSNumber*) const override;
	bool operator > (const LSString*) const override;
	bool operator > (const LSArray*) const override;
	bool operator > (const LSFunction*) const override;
	bool operator > (const LSObject*) const override;
	bool operator > (const LSClass*) const override;

	bool operator <= (const LSValue*) const override;
	bool operator <= (const LSNull*) const override;
	bool operator <= (const LSBoolean*) const override;
	bool operator <= (const LSNumber*) const override;
	bool operator <= (const LSString*) const override;
	bool operator <= (const LSArray*) const override;
	bool operator <= (const LSFunction*) const override;
	bool operator <= (const LSObject*) const override;
	bool operator <= (const LSClass*) const override;

	bool operator >= (const LSValue*) const override;
	bool operator >= (const LSNull*) const override;
	bool operator >= (const LSBoolean*) const override;
	bool operator >= (const LSNumber*) const override;
	bool operator >= (const LSString*) const override;
	bool operator >= (const LSArray*) const override;
	bool operator >= (const LSFunction*) const override;
	bool operator >= (const LSObject*) const override;
	bool operator >= (const LSClass*) const override;

	bool in(const LSValue*) const override;

	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* range(const LSValue* start, const LSValue* end) const override;
	LSValue* rangeL(const LSValue* start, const LSValue* end) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	LSValue* clone() const;

	std::ostream& print(std::ostream& os) const;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override;

	virtual RawType getRawType() const override;
};

#endif
