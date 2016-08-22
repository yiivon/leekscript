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
template <typename T> class LSArray;
template <typename K, typename T> class LSMap;
template <typename T> class LSSet;
class LSFunction;
class LSObject;
class LSClass;
class Context;

#define LSVALUE_OPERATORS \
	LSValue* ls_radd(LSValue* value) override    { return value->ls_add(this); } \
	LSValue* ls_radd_eq(LSValue* value) override { return value->ls_add_eq(this); } \
	LSValue* ls_rsub(LSValue* value) override    { return value->ls_sub(this); } \
	LSValue* ls_rsub_eq(LSValue* value) override { return value->ls_sub_eq(this); }


class LSValue {
public:

	static int obj_count;
	static int obj_deleted;

	int refs = 0;

	LSValue();
	LSValue(const LSValue& other);
	virtual ~LSValue() = 0;

	virtual bool native() const { return false; }

	virtual bool isTrue() const = 0;

	virtual LSValue* operator - () const;
	virtual LSValue* operator ! () const;
	virtual LSValue* operator ~ () const;

	virtual LSValue* operator ++ ();
	virtual LSValue* operator ++ (int);

	virtual LSValue* operator -- ();
	virtual LSValue* operator -- (int);

	LSValue* ls_add(LSValue* value) { return value->ls_radd(this); }
	virtual LSValue* ls_radd(LSValue*) = 0;
	virtual LSValue* ls_add(LSNull*);
	virtual LSValue* ls_add(LSBoolean*);
	virtual LSValue* ls_add(LSNumber*);
	virtual LSValue* ls_add(LSString*);
	virtual LSValue* ls_add(LSArray<LSValue*>*);
	virtual LSValue* ls_add(LSArray<int>*);
	virtual LSValue* ls_add(LSArray<double>*);
	virtual LSValue* ls_add(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_add(LSMap<LSValue*,int>*);
	virtual LSValue* ls_add(LSMap<LSValue*,double>*);
	virtual LSValue* ls_add(LSMap<int,LSValue*>*);
	virtual LSValue* ls_add(LSMap<int,int>*);
	virtual LSValue* ls_add(LSMap<int,double>*);
	virtual LSValue* ls_add(LSSet<LSValue*>*);
	virtual LSValue* ls_add(LSSet<int>*);
	virtual LSValue* ls_add(LSSet<double>*);
	virtual LSValue* ls_add(LSObject*);
	virtual LSValue* ls_add(LSFunction*);
	virtual LSValue* ls_add(LSClass*);

	LSValue* ls_add_eq(LSValue* value) { return value->ls_radd_eq(this); }
	virtual LSValue* ls_radd_eq(LSValue*) = 0;
	virtual LSValue* ls_add_eq(LSNull*);
	virtual LSValue* ls_add_eq(LSBoolean*);
	virtual LSValue* ls_add_eq(LSNumber*);
	virtual LSValue* ls_add_eq(LSString*);
	virtual LSValue* ls_add_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_add_eq(LSArray<int>*);
	virtual LSValue* ls_add_eq(LSArray<double>*);
	virtual LSValue* ls_add_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_add_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_add_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_add_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_add_eq(LSMap<int,int>*);
	virtual LSValue* ls_add_eq(LSMap<int,double>*);
	virtual LSValue* ls_add_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_add_eq(LSSet<int>*);
	virtual LSValue* ls_add_eq(LSSet<double>*);
	virtual LSValue* ls_add_eq(LSObject*);
	virtual LSValue* ls_add_eq(LSFunction*);
	virtual LSValue* ls_add_eq(LSClass*);

	LSValue* ls_sub(LSValue* value) { return value->ls_rsub(this); }
	virtual LSValue* ls_rsub(LSValue*) = 0;
	virtual LSValue* ls_sub(LSNull*);
	virtual LSValue* ls_sub(LSBoolean*);
	virtual LSValue* ls_sub(LSNumber*);
	virtual LSValue* ls_sub(LSString*);
	virtual LSValue* ls_sub(LSArray<LSValue*>*);
	virtual LSValue* ls_sub(LSArray<int>*);
	virtual LSValue* ls_sub(LSArray<double>*);
	virtual LSValue* ls_sub(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_sub(LSMap<LSValue*,int>*);
	virtual LSValue* ls_sub(LSMap<LSValue*,double>*);
	virtual LSValue* ls_sub(LSMap<int,LSValue*>*);
	virtual LSValue* ls_sub(LSMap<int,int>*);
	virtual LSValue* ls_sub(LSMap<int,double>*);
	virtual LSValue* ls_sub(LSSet<LSValue*>*);
	virtual LSValue* ls_sub(LSSet<int>*);
	virtual LSValue* ls_sub(LSSet<double>*);
	virtual LSValue* ls_sub(LSObject*);
	virtual LSValue* ls_sub(LSFunction*);
	virtual LSValue* ls_sub(LSClass*);

	LSValue* ls_sub_eq(LSValue* value) { return value->ls_rsub_eq(this); }
	virtual LSValue* ls_rsub_eq(LSValue*) = 0;
	virtual LSValue* ls_sub_eq(LSNull*);
	virtual LSValue* ls_sub_eq(LSBoolean*);
	virtual LSValue* ls_sub_eq(LSNumber*);
	virtual LSValue* ls_sub_eq(LSString*);
	virtual LSValue* ls_sub_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_sub_eq(LSArray<int>*);
	virtual LSValue* ls_sub_eq(LSArray<double>*);
	virtual LSValue* ls_sub_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_sub_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_sub_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_sub_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_sub_eq(LSMap<int,int>*);
	virtual LSValue* ls_sub_eq(LSMap<int,double>*);
	virtual LSValue* ls_sub_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_sub_eq(LSSet<int>*);
	virtual LSValue* ls_sub_eq(LSSet<double>*);
	virtual LSValue* ls_sub_eq(LSObject*);
	virtual LSValue* ls_sub_eq(LSFunction*);
	virtual LSValue* ls_sub_eq(LSClass*);

	virtual LSValue* operator * (const LSValue*) const = 0;
	virtual LSValue* operator * (const LSNull*) const;
	virtual LSValue* operator * (const LSBoolean*) const;
	virtual LSValue* operator * (const LSNumber*) const;
	virtual LSValue* operator * (const LSString*) const;
	virtual LSValue* operator * (const LSArray<LSValue*>*) const;
	virtual LSValue* operator * (const LSArray<int>*) const;
	virtual LSValue* operator * (const LSArray<double>*) const;
	virtual LSValue* operator * (const LSMap<LSValue*,LSValue*>*) const;
	virtual LSValue* operator * (const LSMap<LSValue*,int>*) const;
	virtual LSValue* operator * (const LSMap<LSValue*,double>*) const;
	virtual LSValue* operator * (const LSMap<int,LSValue*>*) const;
	virtual LSValue* operator * (const LSMap<int,int>*) const;
	virtual LSValue* operator * (const LSMap<int,double>*) const;
	virtual LSValue* operator * (const LSSet<LSValue*>*) const;
	virtual LSValue* operator * (const LSSet<int>*) const;
	virtual LSValue* operator * (const LSSet<double>*) const;
	virtual LSValue* operator * (const LSObject*) const;
	virtual LSValue* operator * (const LSFunction*) const;
	virtual LSValue* operator * (const LSClass*) const;

	virtual LSValue* operator *= (LSValue*) = 0;
	virtual LSValue* operator *= (const LSNull*);
	virtual LSValue* operator *= (const LSBoolean*);
	virtual LSValue* operator *= (const LSNumber*);
	virtual LSValue* operator *= (const LSString*);
	virtual LSValue* operator *= (const LSArray<LSValue*>*);
	virtual LSValue* operator *= (const LSArray<int>*);
	virtual LSValue* operator *= (const LSArray<double>*);
	virtual LSValue* operator *= (const LSMap<LSValue*,LSValue*>*);
	virtual LSValue* operator *= (const LSMap<LSValue*,int>*);
	virtual LSValue* operator *= (const LSMap<LSValue*,double>*);
	virtual LSValue* operator *= (const LSMap<int,LSValue*>*);
	virtual LSValue* operator *= (const LSMap<int,int>*);
	virtual LSValue* operator *= (const LSMap<int,double>*);
	virtual LSValue* operator *= (const LSSet<LSValue*>*);
	virtual LSValue* operator *= (const LSSet<int>*);
	virtual LSValue* operator *= (const LSSet<double>*);
	virtual LSValue* operator *= (const LSObject*);
	virtual LSValue* operator *= (const LSFunction*);
	virtual LSValue* operator *= (const LSClass*);

	virtual LSValue* operator / (const LSValue*) const = 0;
	virtual LSValue* operator / (const LSNull*) const;
	virtual LSValue* operator / (const LSBoolean*) const;
	virtual LSValue* operator / (const LSNumber*) const;
	virtual LSValue* operator / (const LSString*) const;
	virtual LSValue* operator / (const LSArray<LSValue*>*) const;
	virtual LSValue* operator / (const LSArray<int>*) const;
	virtual LSValue* operator / (const LSArray<double>*) const;
	virtual LSValue* operator / (const LSMap<LSValue*,LSValue*>*) const;
	virtual LSValue* operator / (const LSMap<LSValue*,int>*) const;
	virtual LSValue* operator / (const LSMap<LSValue*,double>*) const;
	virtual LSValue* operator / (const LSMap<int,LSValue*>*) const;
	virtual LSValue* operator / (const LSMap<int,int>*) const;
	virtual LSValue* operator / (const LSMap<int,double>*) const;
	virtual LSValue* operator / (const LSSet<LSValue*>*) const;
	virtual LSValue* operator / (const LSSet<int>*) const;
	virtual LSValue* operator / (const LSSet<double>*) const;
	virtual LSValue* operator / (const LSObject*) const;
	virtual LSValue* operator / (const LSFunction*) const;
	virtual LSValue* operator / (const LSClass*) const;

	virtual LSValue* operator /= (LSValue*) = 0;
	virtual LSValue* operator /= (const LSNull*);
	virtual LSValue* operator /= (const LSBoolean*);
	virtual LSValue* operator /= (const LSNumber*);
	virtual LSValue* operator /= (const LSString*);
	virtual LSValue* operator /= (const LSArray<LSValue*>*);
	virtual LSValue* operator /= (const LSArray<int>*);
	virtual LSValue* operator /= (const LSArray<double>*);
	virtual LSValue* operator /= (const LSMap<LSValue*,LSValue*>*);
	virtual LSValue* operator /= (const LSMap<LSValue*,int>*);
	virtual LSValue* operator /= (const LSMap<LSValue*,double>*);
	virtual LSValue* operator /= (const LSMap<int,LSValue*>*);
	virtual LSValue* operator /= (const LSMap<int,int>*);
	virtual LSValue* operator /= (const LSMap<int,double>*);
	virtual LSValue* operator /= (const LSSet<LSValue*>*);
	virtual LSValue* operator /= (const LSSet<int>*);
	virtual LSValue* operator /= (const LSSet<double>*);
	virtual LSValue* operator /= (const LSObject*);
	virtual LSValue* operator /= (const LSFunction*);
	virtual LSValue* operator /= (const LSClass*);

	virtual LSValue* poww(const LSValue*) const = 0;
	virtual LSValue* poww(const LSNull*) const;
	virtual LSValue* poww(const LSBoolean*) const;
	virtual LSValue* poww(const LSNumber*) const;
	virtual LSValue* poww(const LSString*) const;
	virtual LSValue* poww(const LSArray<LSValue*>*) const;
	virtual LSValue* poww(const LSArray<int>*) const;
	virtual LSValue* poww(const LSArray<double>*) const;
	virtual LSValue* poww(const LSMap<LSValue*,LSValue*>*) const;
	virtual LSValue* poww(const LSMap<LSValue*,int>*) const;
	virtual LSValue* poww(const LSMap<LSValue*,double>*) const;
	virtual LSValue* poww(const LSMap<int,LSValue*>*) const;
	virtual LSValue* poww(const LSMap<int,int>*) const;
	virtual LSValue* poww(const LSMap<int,double>*) const;
	virtual LSValue* poww(const LSSet<LSValue*>*) const;
	virtual LSValue* poww(const LSSet<int>*) const;
	virtual LSValue* poww(const LSSet<double>*) const;
	virtual LSValue* poww(const LSObject*) const;
	virtual LSValue* poww(const LSFunction*) const;
	virtual LSValue* poww(const LSClass*) const;

	virtual LSValue* pow_eq(LSValue*) = 0;
	virtual LSValue* pow_eq(const LSNull*);
	virtual LSValue* pow_eq(const LSBoolean*);
	virtual LSValue* pow_eq(const LSNumber*);
	virtual LSValue* pow_eq(const LSString*);
	virtual LSValue* pow_eq(const LSArray<LSValue*>*);
	virtual LSValue* pow_eq(const LSArray<int>*);
	virtual LSValue* pow_eq(const LSArray<double>*);
	virtual LSValue* pow_eq(const LSMap<LSValue*,LSValue*>*);
	virtual LSValue* pow_eq(const LSMap<LSValue*,int>*);
	virtual LSValue* pow_eq(const LSMap<LSValue*,double>*);
	virtual LSValue* pow_eq(const LSMap<int,LSValue*>*);
	virtual LSValue* pow_eq(const LSMap<int,int>*);
	virtual LSValue* pow_eq(const LSMap<int,double>*);
	virtual LSValue* pow_eq(const LSSet<LSValue*>*);
	virtual LSValue* pow_eq(const LSSet<int>*);
	virtual LSValue* pow_eq(const LSSet<double>*);
	virtual LSValue* pow_eq(const LSObject*);
	virtual LSValue* pow_eq(const LSFunction*);
	virtual LSValue* pow_eq(const LSClass*);

	virtual LSValue* operator % (const LSValue*) const = 0;
	virtual LSValue* operator % (const LSNull*) const;
	virtual LSValue* operator % (const LSBoolean*) const;
	virtual LSValue* operator % (const LSNumber*) const;
	virtual LSValue* operator % (const LSString*) const;
	virtual LSValue* operator % (const LSArray<LSValue*>*) const;
	virtual LSValue* operator % (const LSArray<int>*) const;
	virtual LSValue* operator % (const LSArray<double>*) const;
	virtual LSValue* operator % (const LSMap<LSValue*,LSValue*>*) const;
	virtual LSValue* operator % (const LSMap<LSValue*,int>*) const;
	virtual LSValue* operator % (const LSMap<LSValue*,double>*) const;
	virtual LSValue* operator % (const LSMap<int,LSValue*>*) const;
	virtual LSValue* operator % (const LSMap<int,int>*) const;
	virtual LSValue* operator % (const LSMap<int,double>*) const;
	virtual LSValue* operator % (const LSSet<LSValue*>*) const;
	virtual LSValue* operator % (const LSSet<int>*) const;
	virtual LSValue* operator % (const LSSet<double>*) const;
	virtual LSValue* operator % (const LSObject*) const;
	virtual LSValue* operator % (const LSFunction*) const;
	virtual LSValue* operator % (const LSClass*) const;

	virtual LSValue* operator %= (LSValue*) = 0;
	virtual LSValue* operator %= (const LSNull*);
	virtual LSValue* operator %= (const LSBoolean*);
	virtual LSValue* operator %= (const LSNumber*);
	virtual LSValue* operator %= (const LSString*);
	virtual LSValue* operator %= (const LSArray<LSValue*>*);
	virtual LSValue* operator %= (const LSArray<int>*);
	virtual LSValue* operator %= (const LSArray<double>*);
	virtual LSValue* operator %= (const LSMap<LSValue*,LSValue*>*);
	virtual LSValue* operator %= (const LSMap<LSValue*,int>*);
	virtual LSValue* operator %= (const LSMap<LSValue*,double>*);
	virtual LSValue* operator %= (const LSMap<int,LSValue*>*);
	virtual LSValue* operator %= (const LSMap<int,int>*);
	virtual LSValue* operator %= (const LSMap<int,double>*);
	virtual LSValue* operator %= (const LSSet<LSValue*>*);
	virtual LSValue* operator %= (const LSSet<int>*);
	virtual LSValue* operator %= (const LSSet<double>*);
	virtual LSValue* operator %= (const LSObject*);
	virtual LSValue* operator %= (const LSFunction*);
	virtual LSValue* operator %= (const LSClass*);

	virtual bool operator == (const LSValue*) const = 0;
	virtual bool operator == (const LSNull*) const;
	virtual bool operator == (const LSBoolean*) const;
	virtual bool operator == (const LSNumber*) const;
	virtual bool operator == (const LSString*) const;
	virtual bool operator == (const LSArray<LSValue*>*) const;
	virtual bool operator == (const LSArray<int>*) const;
	virtual bool operator == (const LSArray<double>*) const;
	virtual bool operator == (const LSMap<LSValue*,LSValue*>*) const;
	virtual bool operator == (const LSMap<LSValue*,int>*) const;
	virtual bool operator == (const LSMap<LSValue*,double>*) const;
	virtual bool operator == (const LSMap<int,LSValue*>*) const;
	virtual bool operator == (const LSMap<int,int>*) const;
	virtual bool operator == (const LSMap<int,double>*) const;
	virtual bool operator == (const LSSet<LSValue*>*) const;
	virtual bool operator == (const LSSet<int>*) const;
	virtual bool operator == (const LSSet<double>*) const;
	virtual bool operator == (const LSFunction*) const;
	virtual bool operator == (const LSObject*) const;
	virtual bool operator == (const LSClass*) const;

	inline bool operator != (const LSValue* value) const {
		return not this->operator ==(value);
	}

	virtual bool operator < (const LSValue*) const = 0;
	virtual bool operator < (const LSNull*) const;
	virtual bool operator < (const LSBoolean*) const;
	virtual bool operator < (const LSNumber*) const;
	virtual bool operator < (const LSString*) const;
	virtual bool operator < (const LSArray<LSValue*>*) const;
	virtual bool operator < (const LSArray<int>*) const;
	virtual bool operator < (const LSArray<double>*) const;
	virtual bool operator < (const LSMap<LSValue*,LSValue*>*) const;
	virtual bool operator < (const LSMap<LSValue*,int>*) const;
	virtual bool operator < (const LSMap<LSValue*,double>*) const;
	virtual bool operator < (const LSMap<int,LSValue*>*) const;
	virtual bool operator < (const LSMap<int,int>*) const;
	virtual bool operator < (const LSMap<int,double>*) const;
	virtual bool operator < (const LSSet<LSValue*>*) const;
	virtual bool operator < (const LSSet<int>*) const;
	virtual bool operator < (const LSSet<double>*) const;
	virtual bool operator < (const LSFunction*) const;
	virtual bool operator < (const LSObject*) const;
	virtual bool operator < (const LSClass*) const;

	inline virtual bool operator > (const LSValue* value) const {
		return not this->operator <(value) and not this->operator ==(value);
	}
	virtual bool operator > (const LSNull*) const;
	virtual bool operator > (const LSBoolean*) const;
	virtual bool operator > (const LSNumber*) const;
	virtual bool operator > (const LSString*) const;
	virtual bool operator > (const LSArray<LSValue*>*) const;
	virtual bool operator > (const LSArray<int>*) const;
	virtual bool operator > (const LSArray<double>*) const;
	virtual bool operator > (const LSMap<LSValue*,LSValue*>*) const;
	virtual bool operator > (const LSMap<LSValue*,int>*) const;
	virtual bool operator > (const LSMap<LSValue*,double>*) const;
	virtual bool operator > (const LSMap<int,LSValue*>*) const;
	virtual bool operator > (const LSMap<int,int>*) const;
	virtual bool operator > (const LSMap<int,double>*) const;
	virtual bool operator > (const LSSet<LSValue*>*) const;
	virtual bool operator > (const LSSet<int>*) const;
	virtual bool operator > (const LSSet<double>*) const;
	virtual bool operator > (const LSFunction*) const;
	virtual bool operator > (const LSObject*) const;
	virtual bool operator > (const LSClass*) const;

	inline bool operator <=(const LSValue* value) const {
		return this->operator <(value) || this->operator ==(value);
	}
	inline bool operator >=(const LSValue*value) const {
		return not this->operator <(value);
	}

	virtual bool in(LSValue*) const;

	virtual LSValue* at(const LSValue* key) const;
	virtual LSValue** atL(const LSValue* key);

	virtual LSValue* attr(const LSValue* key) const;
	virtual LSValue** attrL(const LSValue* key);

	virtual LSValue* range(int start, int end) const;
	virtual LSValue* rangeL(int start, int end);

	virtual LSValue* abso() const;

	virtual std::ostream& print(std::ostream&) const = 0;
	virtual std::string json() const = 0;
	std::string to_json() const;

	virtual LSValue* clone() const = 0;
	LSValue* clone_inc();
	LSValue* move();
	LSValue* move_inc();

	virtual LSValue* getClass() const = 0;

	bool isInteger() const;

	virtual int typeID() const = 0;

	virtual const BaseRawType* getRawType() const = 0;

	static LSValue* parse(Json& json);

	static void delete_ref(LSValue* value);
	static void delete_temporary(LSValue* value);
};

inline LSValue* LSValue::clone_inc() {
	if (native()) {
		return this;
	} else {
		LSValue* copy = clone();
		copy->refs++;
		return copy;
	}
}

inline LSValue* LSValue::move() {
	if (native()) {
		return this;
	} else {
		if (refs == 0) {
			return this;
		}
		return clone();
	}
}

inline LSValue* LSValue::move_inc() {
	if (native()) {
		return this;
	} else if (refs == 0) {
		refs++;
		return this;
	} else {
		LSValue* copy = clone();
		copy->refs++;
		return copy;
	}
}

inline void LSValue::delete_ref(LSValue* value) {

	if (value == nullptr) return;
	if (value->native()) return;
	if (value->refs == 0) return;

	value->refs--;
	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::delete_temporary(LSValue* value)
{
	if (value == nullptr) return;
	if (value->native()) return;

	if (value->refs == 0) {
		delete value;
	}
}

}

#endif

