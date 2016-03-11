#ifndef LSVALUE_H_
#define LSVALUE_H_

#include <iostream>
#include <string>
#include <cstddef>
#include "../lib/gason.h"
#include "Type.hpp"

class LSVar;
class LSNull;
class LSBoolean;
class LSNumber;
class LSString;
class LSArray;
class LSFunction;
class LSObject;
class LSClass;
class Context;

class LSValue {
public:

	virtual ~LSValue() = 0;

	virtual bool isTrue() const = 0;

	virtual LSValue* operator - () const = 0;
	virtual LSValue* operator ! () const = 0;
	virtual LSValue* operator ~ () const = 0;

	virtual LSValue* operator ++ () = 0;
	virtual LSValue* operator ++ (int) = 0;

	virtual LSValue* operator -- () = 0;
	virtual LSValue* operator -- (int) = 0;

	virtual LSValue* operator + (const LSValue*) const = 0;
	virtual LSValue* operator + (const LSNull*) const = 0;
	virtual LSValue* operator + (const LSBoolean*) const = 0;
	virtual LSValue* operator + (const LSNumber*) const = 0;
	virtual LSValue* operator + (const LSString*) const = 0;
	virtual LSValue* operator + (const LSArray*) const = 0;
	virtual LSValue* operator + (const LSObject*) const = 0;
	virtual LSValue* operator + (const LSFunction*) const = 0;
	virtual LSValue* operator + (const LSClass*) const = 0;

	virtual LSValue* operator += (LSValue*) const = 0;
	virtual LSValue* operator += (const LSNull*) = 0;
	virtual LSValue* operator += (const LSBoolean*) = 0;
	virtual LSValue* operator += (const LSNumber*) = 0;
	virtual LSValue* operator += (const LSString*) = 0;
	virtual LSValue* operator += (const LSArray*) = 0;
	virtual LSValue* operator += (const LSObject*) = 0;
	virtual LSValue* operator += (const LSFunction*) = 0;
	virtual LSValue* operator += (const LSClass*) = 0;

	virtual LSValue* operator - (const LSValue*) const = 0;
	virtual LSValue* operator - (const LSNull*) const = 0;
	virtual LSValue* operator - (const LSBoolean*) const = 0;
	virtual LSValue* operator - (const LSNumber*) const = 0;
	virtual LSValue* operator - (const LSString*) const = 0;
	virtual LSValue* operator - (const LSArray*) const = 0;
	virtual LSValue* operator - (const LSObject*) const = 0;
	virtual LSValue* operator - (const LSFunction*) const = 0;
	virtual LSValue* operator - (const LSClass*) const = 0;

	virtual LSValue* operator -= (LSValue*) const = 0;
	virtual LSValue* operator -= (const LSNull*) = 0;
	virtual LSValue* operator -= (const LSBoolean*) = 0;
	virtual LSValue* operator -= (const LSNumber*) = 0;
	virtual LSValue* operator -= (const LSString*) = 0;
	virtual LSValue* operator -= (const LSArray*) = 0;
	virtual LSValue* operator -= (const LSObject*) = 0;
	virtual LSValue* operator -= (const LSFunction*) = 0;
	virtual LSValue* operator -= (const LSClass*) = 0;

	virtual LSValue* operator * (const LSValue*) const = 0;
	virtual LSValue* operator * (const LSNull*) const = 0;
	virtual LSValue* operator * (const LSBoolean*) const = 0;
	virtual LSValue* operator * (const LSNumber*) const = 0;
	virtual LSValue* operator * (const LSString*) const = 0;
	virtual LSValue* operator * (const LSArray*) const = 0;
	virtual LSValue* operator * (const LSObject*) const = 0;
	virtual LSValue* operator * (const LSFunction*) const = 0;
	virtual LSValue* operator * (const LSClass*) const = 0;

	virtual LSValue* operator *= (LSValue*) const = 0;
	virtual LSValue* operator *= (const LSNull*) = 0;
	virtual LSValue* operator *= (const LSBoolean*) = 0;
	virtual LSValue* operator *= (const LSNumber*) = 0;
	virtual LSValue* operator *= (const LSString*) = 0;
	virtual LSValue* operator *= (const LSArray*) = 0;
	virtual LSValue* operator *= (const LSObject*) = 0;
	virtual LSValue* operator *= (const LSFunction*) = 0;
	virtual LSValue* operator *= (const LSClass*) = 0;

	virtual LSValue* operator / (const LSValue*) const = 0;
	virtual LSValue* operator / (const LSNull*) const = 0;
	virtual LSValue* operator / (const LSBoolean*) const = 0;
	virtual LSValue* operator / (const LSNumber*) const = 0;
	virtual LSValue* operator / (const LSString*) const = 0;
	virtual LSValue* operator / (const LSArray*) const = 0;
	virtual LSValue* operator / (const LSObject*) const = 0;
	virtual LSValue* operator / (const LSFunction*) const = 0;
	virtual LSValue* operator / (const LSClass*) const = 0;

	virtual LSValue* operator /= (LSValue*) const = 0;
	virtual LSValue* operator /= (const LSNull*) = 0;
	virtual LSValue* operator /= (const LSBoolean*) = 0;
	virtual LSValue* operator /= (const LSNumber*) = 0;
	virtual LSValue* operator /= (const LSString*) = 0;
	virtual LSValue* operator /= (const LSArray*) = 0;
	virtual LSValue* operator /= (const LSObject*) = 0;
	virtual LSValue* operator /= (const LSFunction*) = 0;
	virtual LSValue* operator /= (const LSClass*) = 0;

	virtual LSValue* poww(const LSValue*) const = 0;
	virtual LSValue* poww(const LSNull*) const = 0;
	virtual LSValue* poww(const LSBoolean*) const = 0;
	virtual LSValue* poww(const LSNumber*) const = 0;
	virtual LSValue* poww(const LSString*) const = 0;
	virtual LSValue* poww(const LSArray*) const = 0;
	virtual LSValue* poww(const LSObject*) const = 0;
	virtual LSValue* poww(const LSFunction*) const = 0;
	virtual LSValue* poww(const LSClass*) const = 0;

	virtual LSValue* pow_eq(LSValue*) const = 0;
	virtual LSValue* pow_eq(const LSNull*) = 0;
	virtual LSValue* pow_eq(const LSBoolean*) = 0;
	virtual LSValue* pow_eq(const LSNumber*) = 0;
	virtual LSValue* pow_eq(const LSString*) = 0;
	virtual LSValue* pow_eq(const LSArray*) = 0;
	virtual LSValue* pow_eq(const LSObject*) = 0;
	virtual LSValue* pow_eq(const LSFunction*) = 0;
	virtual LSValue* pow_eq(const LSClass*) = 0;

	virtual LSValue* operator % (const LSValue*) const = 0;
	virtual LSValue* operator % (const LSNull*) const = 0;
	virtual LSValue* operator % (const LSBoolean*) const = 0;
	virtual LSValue* operator % (const LSNumber*) const = 0;
	virtual LSValue* operator % (const LSString*) const = 0;
	virtual LSValue* operator % (const LSArray*) const = 0;
	virtual LSValue* operator % (const LSObject*) const = 0;
	virtual LSValue* operator % (const LSFunction*) const = 0;
	virtual LSValue* operator % (const LSClass*) const = 0;

	virtual LSValue* operator %= (LSValue*) const = 0;
	virtual LSValue* operator %= (const LSNull*) = 0;
	virtual LSValue* operator %= (const LSBoolean*) = 0;
	virtual LSValue* operator %= (const LSNumber*) = 0;
	virtual LSValue* operator %= (const LSString*) = 0;
	virtual LSValue* operator %= (const LSArray*) = 0;
	virtual LSValue* operator %= (const LSObject*) = 0;
	virtual LSValue* operator %= (const LSFunction*) = 0;
	virtual LSValue* operator %= (const LSClass*) = 0;

	virtual bool operator == (const LSValue*) const = 0;
	virtual bool operator == (const LSNull*) const = 0;
	virtual bool operator == (const LSBoolean*) const = 0;
	virtual bool operator == (const LSNumber*) const = 0;
	virtual bool operator == (const LSString*) const = 0;
	virtual bool operator == (const LSArray*) const = 0;
	virtual bool operator == (const LSFunction*) const = 0;
	virtual bool operator == (const LSObject*) const = 0;
	virtual bool operator == (const LSClass*) const = 0;

	virtual bool operator < (const LSValue*) const = 0;
	virtual bool operator < (const LSNull*) const = 0;
	virtual bool operator < (const LSBoolean*) const = 0;
	virtual bool operator < (const LSNumber*) const = 0;
	virtual bool operator < (const LSString*) const = 0;
	virtual bool operator < (const LSArray*) const = 0;
	virtual bool operator < (const LSFunction*) const = 0;
	virtual bool operator < (const LSObject*) const = 0;
	virtual bool operator < (const LSClass*) const = 0;

	virtual bool operator > (const LSValue*) const = 0;
	virtual bool operator > (const LSNull*) const = 0;
	virtual bool operator > (const LSBoolean*) const = 0;
	virtual bool operator > (const LSNumber*) const = 0;
	virtual bool operator > (const LSString*) const = 0;
	virtual bool operator > (const LSArray*) const = 0;
	virtual bool operator > (const LSFunction*) const = 0;
	virtual bool operator > (const LSObject*) const = 0;
	virtual bool operator > (const LSClass*) const = 0;

	virtual bool operator <= (const LSValue*) const = 0;
	virtual bool operator <= (const LSNull*) const = 0;
	virtual bool operator <= (const LSBoolean*) const = 0;
	virtual bool operator <= (const LSNumber*) const = 0;
	virtual bool operator <= (const LSString*) const = 0;
	virtual bool operator <= (const LSArray*) const = 0;
	virtual bool operator <= (const LSFunction*) const = 0;
	virtual bool operator <= (const LSObject*) const = 0;
	virtual bool operator <= (const LSClass*) const = 0;

	virtual bool operator >= (const LSValue*) const = 0;
	virtual bool operator >= (const LSNull*) const = 0;
	virtual bool operator >= (const LSBoolean*) const = 0;
	virtual bool operator >= (const LSNumber*) const = 0;
	virtual bool operator >= (const LSString*) const = 0;
	virtual bool operator >= (const LSArray*) const = 0;
	virtual bool operator >= (const LSFunction*) const = 0;
	virtual bool operator >= (const LSObject*) const = 0;
	virtual bool operator >= (const LSClass*) const = 0;

	virtual bool in(const LSValue*) const = 0;

	virtual LSValue* at(const LSValue* key) const = 0;
	virtual LSValue** atL(const LSValue* key) = 0;

	virtual LSValue* attr(const LSValue* key) const = 0;
	virtual LSValue** attrL(const LSValue* key) = 0;

	virtual LSValue* range(const LSValue* start, const LSValue* end) const = 0;
	virtual LSValue* rangeL(const LSValue* start, const LSValue* end) = 0;

	virtual LSValue* abso() const = 0;

	virtual std::ostream& print(std::ostream&) const = 0;
	virtual std::string json() const = 0;
	std::string to_json() const;

	virtual LSValue* clone() const = 0;

	virtual LSValue* getClass() const = 0;

	bool operator != (const LSValue*) const;

	bool isInteger() const;

	virtual int typeID() const = 0;

	virtual RawType getRawType() const = 0;

	static LSValue* parse(JsonValue& json);
};

inline LSValue::~LSValue() { }

#endif

