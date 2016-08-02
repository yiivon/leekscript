#ifndef LSVALUE_H_
#define LSVALUE_H_

#include <iostream>
#include <string>
#include <cstddef>
#include <map>

#include "../../lib/json.hpp"
#include "Type.hpp"

namespace ls {

class LSVar;
class LSNull;
class LSBoolean;
class LSNumber;
class LSString;
template <class T> class LSArray;
class LSFunction;
class LSObject;
class LSClass;
class Context;

class LSValue {
public:

	static int obj_count;
	static int obj_deleted;

	int refs = 0;
	bool native;

	LSValue();
	virtual ~LSValue() = 0;

	virtual bool isTrue() const = 0;

	virtual LSValue* operator - () const;
	virtual LSValue* operator ! () const;
	virtual LSValue* operator ~ () const;

	virtual LSValue* operator ++ ();
	virtual LSValue* operator ++ (int);

	virtual LSValue* operator -- ();
	virtual LSValue* operator -- (int);

	virtual LSValue* operator + (const LSValue*) const = 0;
	virtual LSValue* operator + (const LSNull*) const;
	virtual LSValue* operator + (const LSBoolean*) const;
	virtual LSValue* operator + (const LSNumber*) const;
	virtual LSValue* operator + (const LSString*) const;
	virtual LSValue* operator + (const LSArray<LSValue*>*) const;
	virtual LSValue* operator + (const LSArray<int>*) const;
	virtual LSValue* operator + (const LSArray<double>*) const;
	virtual LSValue* operator + (const LSObject*) const;
	virtual LSValue* operator + (const LSFunction*) const;
	virtual LSValue* operator + (const LSClass*) const;

	virtual LSValue* operator += (LSValue*) = 0;
	virtual LSValue* operator += (const LSNull*);
	virtual LSValue* operator += (const LSBoolean*);
	virtual LSValue* operator += (const LSNumber*);
	virtual LSValue* operator += (const LSString*);
	virtual LSValue* operator += (const LSArray<LSValue*>*);
	virtual LSValue* operator += (const LSObject*);
	virtual LSValue* operator += (const LSFunction*);
	virtual LSValue* operator += (const LSClass*);

	virtual LSValue* operator - (const LSValue*) const = 0;
	virtual LSValue* operator - (const LSNull*) const;
	virtual LSValue* operator - (const LSBoolean*) const;
	virtual LSValue* operator - (const LSNumber*) const;
	virtual LSValue* operator - (const LSString*) const;
	virtual LSValue* operator - (const LSArray<LSValue*>*) const;
	virtual LSValue* operator - (const LSObject*) const;
	virtual LSValue* operator - (const LSFunction*) const;
	virtual LSValue* operator - (const LSClass*) const;

	virtual LSValue* operator -= (LSValue*) = 0;
	virtual LSValue* operator -= (const LSNull*);
	virtual LSValue* operator -= (const LSBoolean*);
	virtual LSValue* operator -= (const LSNumber*);
	virtual LSValue* operator -= (const LSString*);
	virtual LSValue* operator -= (const LSArray<LSValue*>*);
	virtual LSValue* operator -= (const LSObject*);
	virtual LSValue* operator -= (const LSFunction*);
	virtual LSValue* operator -= (const LSClass*);

	virtual LSValue* operator * (const LSValue*) const = 0;
	virtual LSValue* operator * (const LSNull*) const;
	virtual LSValue* operator * (const LSBoolean*) const;
	virtual LSValue* operator * (const LSNumber*) const;
	virtual LSValue* operator * (const LSString*) const;
	virtual LSValue* operator * (const LSArray<LSValue*>*) const;
	virtual LSValue* operator * (const LSObject*) const;
	virtual LSValue* operator * (const LSFunction*) const;
	virtual LSValue* operator * (const LSClass*) const;

	virtual LSValue* operator *= (LSValue*) = 0;
	virtual LSValue* operator *= (const LSNull*);
	virtual LSValue* operator *= (const LSBoolean*);
	virtual LSValue* operator *= (const LSNumber*);
	virtual LSValue* operator *= (const LSString*);
	virtual LSValue* operator *= (const LSArray<LSValue*>*);
	virtual LSValue* operator *= (const LSObject*);
	virtual LSValue* operator *= (const LSFunction*);
	virtual LSValue* operator *= (const LSClass*);

	virtual LSValue* operator / (const LSValue*) const = 0;
	virtual LSValue* operator / (const LSNull*) const;
	virtual LSValue* operator / (const LSBoolean*) const;
	virtual LSValue* operator / (const LSNumber*) const;
	virtual LSValue* operator / (const LSString*) const;
	virtual LSValue* operator / (const LSArray<LSValue*>*) const;
	virtual LSValue* operator / (const LSObject*) const;
	virtual LSValue* operator / (const LSFunction*) const;
	virtual LSValue* operator / (const LSClass*) const;

	virtual LSValue* operator /= (LSValue*) = 0;
	virtual LSValue* operator /= (const LSNull*);
	virtual LSValue* operator /= (const LSBoolean*);
	virtual LSValue* operator /= (const LSNumber*);
	virtual LSValue* operator /= (const LSString*);
	virtual LSValue* operator /= (const LSArray<LSValue*>*);
	virtual LSValue* operator /= (const LSObject*);
	virtual LSValue* operator /= (const LSFunction*);
	virtual LSValue* operator /= (const LSClass*);

	virtual LSValue* poww(const LSValue*) const = 0;
	virtual LSValue* poww(const LSNull*) const;
	virtual LSValue* poww(const LSBoolean*) const;
	virtual LSValue* poww(const LSNumber*) const;
	virtual LSValue* poww(const LSString*) const;
	virtual LSValue* poww(const LSArray<LSValue*>*) const;
	virtual LSValue* poww(const LSObject*) const;
	virtual LSValue* poww(const LSFunction*) const;
	virtual LSValue* poww(const LSClass*) const;

	virtual LSValue* pow_eq(LSValue*) = 0;
	virtual LSValue* pow_eq(const LSNull*);
	virtual LSValue* pow_eq(const LSBoolean*);
	virtual LSValue* pow_eq(const LSNumber*);
	virtual LSValue* pow_eq(const LSString*);
	virtual LSValue* pow_eq(const LSArray<LSValue*>*);
	virtual LSValue* pow_eq(const LSObject*);
	virtual LSValue* pow_eq(const LSFunction*);
	virtual LSValue* pow_eq(const LSClass*);

	virtual LSValue* operator % (const LSValue*) const = 0;
	virtual LSValue* operator % (const LSNull*) const;
	virtual LSValue* operator % (const LSBoolean*) const;
	virtual LSValue* operator % (const LSNumber*) const;
	virtual LSValue* operator % (const LSString*) const;
	virtual LSValue* operator % (const LSArray<LSValue*>*) const;
	virtual LSValue* operator % (const LSObject*) const;
	virtual LSValue* operator % (const LSFunction*) const;
	virtual LSValue* operator % (const LSClass*) const;

	virtual LSValue* operator %= (LSValue*) = 0;
	virtual LSValue* operator %= (const LSNull*);
	virtual LSValue* operator %= (const LSBoolean*);
	virtual LSValue* operator %= (const LSNumber*);
	virtual LSValue* operator %= (const LSString*);
	virtual LSValue* operator %= (const LSArray<LSValue*>*);
	virtual LSValue* operator %= (const LSObject*);
	virtual LSValue* operator %= (const LSFunction*);
	virtual LSValue* operator %= (const LSClass*);

	virtual bool operator == (const LSValue*) const = 0;
	virtual bool operator == (const LSNull*) const = 0;
	virtual bool operator == (const LSBoolean*) const = 0;
	virtual bool operator == (const LSNumber*) const = 0;
	virtual bool operator == (const LSString*) const = 0;
	virtual bool operator == (const LSArray<LSValue*>*) const = 0;
	virtual bool operator == (const LSFunction*) const = 0;
	virtual bool operator == (const LSObject*) const = 0;
	virtual bool operator == (const LSClass*) const = 0;

	virtual bool operator < (const LSValue*) const = 0;
	virtual bool operator < (const LSNull*) const = 0;
	virtual bool operator < (const LSBoolean*) const = 0;
	virtual bool operator < (const LSNumber*) const = 0;
	virtual bool operator < (const LSString*) const = 0;
	virtual bool operator < (const LSArray<LSValue*>*) const = 0;
	virtual bool operator < (const LSArray<int>*) const = 0;
	virtual bool operator < (const LSArray<double>*) const = 0;
	virtual bool operator < (const LSFunction*) const = 0;
	virtual bool operator < (const LSObject*) const = 0;
	virtual bool operator < (const LSClass*) const = 0;

	virtual bool operator > (const LSValue*) const = 0;
	virtual bool operator > (const LSNull*) const = 0;
	virtual bool operator > (const LSBoolean*) const = 0;
	virtual bool operator > (const LSNumber*) const = 0;
	virtual bool operator > (const LSString*) const = 0;
	virtual bool operator > (const LSArray<LSValue*>*) const = 0;
	virtual bool operator > (const LSFunction*) const = 0;
	virtual bool operator > (const LSObject*) const = 0;
	virtual bool operator > (const LSClass*) const = 0;

	virtual bool operator <= (const LSValue*) const = 0;
	virtual bool operator <= (const LSNull*) const = 0;
	virtual bool operator <= (const LSBoolean*) const = 0;
	virtual bool operator <= (const LSNumber*) const = 0;
	virtual bool operator <= (const LSString*) const = 0;
	virtual bool operator <= (const LSArray<LSValue*>*) const = 0;
	virtual bool operator <= (const LSFunction*) const = 0;
	virtual bool operator <= (const LSObject*) const = 0;
	virtual bool operator <= (const LSClass*) const = 0;

	virtual bool operator >= (const LSValue*) const = 0;
	virtual bool operator >= (const LSNull*) const = 0;
	virtual bool operator >= (const LSBoolean*) const = 0;
	virtual bool operator >= (const LSNumber*) const = 0;
	virtual bool operator >= (const LSString*) const = 0;
	virtual bool operator >= (const LSArray<LSValue*>*) const = 0;
	virtual bool operator >= (const LSFunction*) const = 0;
	virtual bool operator >= (const LSObject*) const = 0;
	virtual bool operator >= (const LSClass*) const = 0;

	virtual bool in(const LSValue*) const = 0;

	virtual LSValue* at(const LSValue* key) const = 0;
	virtual LSValue** atL(const LSValue* key) = 0;

	virtual LSValue* attr(const LSValue* key) const = 0;
	virtual LSValue** attrL(const LSValue* key) = 0;

	virtual LSValue* range(int start, int end) const = 0;
	virtual LSValue* rangeL(int start, int end) = 0;

	virtual LSValue* abso() const = 0;

	virtual std::ostream& print(std::ostream&) const = 0;
	virtual std::string json() const = 0;
	std::string to_json() const;

	virtual LSValue* clone() const = 0;

	virtual LSValue* getClass() const = 0;

	bool operator != (const LSValue*) const;

	bool isInteger() const;

	virtual int typeID() const = 0;

	virtual const BaseRawType* getRawType() const = 0;

	static LSValue* parse(Json& json);

	static void delete_val(LSValue* value);
};

}

#endif

