#ifndef LSVALUE_H_
#define LSVALUE_H_

#include <iostream>
#include <string>
#include <cstddef>
#include <map>

#include "../constants.h"
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
template <class T> class LSFunction;
class LSObject;
class LSClass;
class Context;

#define LSVALUE_OPERATORS \
	bool req(const LSValue* value) const override { return value->eq(this); } \
	bool rlt(const LSValue* value) const override { return value->lt(this); } \

class LSValue {
public:

	static int obj_count;
	static int obj_deleted;
	#if DEBUG_LEAKS_DETAILS
		static std::map<void*, LSValue*>& objs() {
			static std::map<void*, LSValue*> objs;
			return objs;
		}
	#endif

	int refs = 0;
	bool native = false;

	LSValue();
	LSValue(const LSValue& other);
	virtual ~LSValue() = 0;

	virtual bool isTrue() const = 0;

	virtual LSValue* ls_minus();
	virtual LSValue* ls_not();
	virtual LSValue* ls_tilde();
	virtual LSValue* ls_preinc(); // ++x
	virtual LSValue* ls_inc(); // x++
	virtual LSValue* ls_predec();
	virtual LSValue* ls_dec();

	virtual LSValue* add(LSValue*);
	virtual LSValue* add_eq(LSValue*);
	virtual LSValue* sub(LSValue*);
	virtual LSValue* sub_eq(LSValue*);
	virtual LSValue* mul(LSValue*);
	virtual LSValue* mul_eq(LSValue*);
	virtual LSValue* div(LSValue*);
	virtual LSValue* div_eq(LSValue*);
	virtual LSValue* int_div(LSValue*);
	virtual LSValue* int_div_eq(LSValue*);
	virtual LSValue* pow(LSValue*);
	virtual LSValue* pow_eq(LSValue*);
	virtual LSValue* mod(LSValue*);
	virtual LSValue* mod_eq(LSValue*);

	bool operator == (const LSValue& value) const {
		return value.req(this);
	}
	virtual bool operator == (int) const { return false; }
	virtual bool operator == (double) const { return false; }

	bool operator != (const LSValue& value) const {
		return !value.req(this);
	}

	virtual bool req(const LSValue*) const = 0;
	virtual bool eq(const LSNull*) const;
	virtual bool eq(const LSBoolean*) const;
	virtual bool eq(const LSNumber*) const;
	virtual bool eq(const LSString*) const;
	virtual bool eq(const LSArray<LSValue*>*) const;
	virtual bool eq(const LSArray<int>*) const;
	virtual bool eq(const LSArray<double>*) const;
	virtual bool eq(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool eq(const LSMap<LSValue*,int>*) const;
	virtual bool eq(const LSMap<LSValue*,double>*) const;
	virtual bool eq(const LSMap<int,LSValue*>*) const;
	virtual bool eq(const LSMap<int,int>*) const;
	virtual bool eq(const LSMap<int,double>*) const;
	virtual bool eq(const LSMap<double,LSValue*>*) const;
	virtual bool eq(const LSMap<double,int>*) const;
	virtual bool eq(const LSMap<double,double>*) const;
	virtual bool eq(const LSSet<LSValue*>*) const;
	virtual bool eq(const LSSet<int>*) const;
	virtual bool eq(const LSSet<double>*) const;
	virtual bool eq(const LSFunction<LSValue*>*) const;
	virtual bool eq(const LSObject*) const;
	virtual bool eq(const LSClass*) const;

	bool operator < (const LSValue& value) const {
		return value.rlt(this);
	}
	bool operator > (const LSValue& value) const {
		return !(*this == value) && !(*this < value);
	}
	bool operator <=(const LSValue& value) const {
		return (*this == value) || (*this < value);
	}
	bool operator >=(const LSValue& value) const {
		return !(*this < value);
	}
	virtual bool rlt(const LSValue*) const = 0;
	virtual bool lt(const LSNull*) const;
	virtual bool lt(const LSBoolean*) const;
	virtual bool lt(const LSNumber*) const;
	virtual bool lt(const LSString*) const;
	virtual bool lt(const LSArray<LSValue*>*) const;
	virtual bool lt(const LSArray<int>*) const;
	virtual bool lt(const LSArray<double>*) const;
	virtual bool lt(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool lt(const LSMap<LSValue*,int>*) const;
	virtual bool lt(const LSMap<LSValue*,double>*) const;
	virtual bool lt(const LSMap<int,LSValue*>*) const;
	virtual bool lt(const LSMap<int,int>*) const;
	virtual bool lt(const LSMap<int,double>*) const;
	virtual bool lt(const LSMap<double,LSValue*>*) const;
	virtual bool lt(const LSMap<double,int>*) const;
	virtual bool lt(const LSMap<double,double>*) const;
	virtual bool lt(const LSSet<LSValue*>*) const;
	virtual bool lt(const LSSet<int>*) const;
	virtual bool lt(const LSSet<double>*) const;
	virtual bool lt(const LSFunction<LSValue*>*) const;
	virtual bool lt(const LSObject*) const;
	virtual bool lt(const LSClass*) const;

	virtual bool in(const LSValue*) const { return false; }

	virtual LSValue* at(const LSValue* key) const;
	virtual LSValue** atL(const LSValue* key);

	virtual LSValue* attr(const std::string& key) const;
	virtual LSValue** attrL(const std::string& key);

	virtual LSValue* range(int start, int end) const;
	virtual LSValue* rangeL(int start, int end);

	virtual LSValue* abso() const;

	virtual int integer() const { return 0; }
	virtual double real() const { return 0; }

	virtual std::string to_string() const;
	virtual std::ostream& print(std::ostream&) const;
	virtual std::ostream& dump(std::ostream&) const = 0;
	virtual std::string json() const;

	LSString* ls_json();

	virtual LSValue* clone() const;
	LSValue* clone_inc();
	LSValue* move();
	LSValue* move_inc();

	virtual LSValue* getClass() const = 0;

	bool isInteger() const;

	virtual int typeID() const = 0;

	template <class T> static LSValue* get(T v);
	static LSValue* parse(Json& json);
	static LSValue* get_from_json(Json& json);

	static void delete_ref(LSValue* value);
	static void delete_temporary(const LSValue* const value);
};

template <> LSValue* LSValue::get(int v);
template <> LSValue* LSValue::get(double v);

inline LSValue* LSValue::clone_inc() {
	if (native) {
		return this;
	} else {
		LSValue* copy = clone();
		copy->refs = 1;
		return copy;
	}
}

inline LSValue* LSValue::move() {
	if (native) {
		return this;
	}
	if (refs == 0) {
		return this;
	}
	return clone();
}

inline LSValue* LSValue::move_inc() {
	if (!native) {
		refs++;
	}
	return this;
}

inline void LSValue::delete_ref(LSValue* value) {

	if (value == nullptr) return;
	if (value->native) return;
	if (value->refs == 0) return;

	value->refs--;
	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::delete_temporary(const LSValue* const value) {

	if (value == nullptr) return;
	if (value->native) return;

	if (value->refs == 0) {
		delete value;
	}
}

inline std::ostream& operator << (std::ostream& os, const LSValue& value) {
	return value.print(os);
}

}

namespace ls {
	template <typename T>
	std::string to_json(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string to_json(LSValue* v) {
		return v->json();
	}

	template <typename T>
	std::string print(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string print(LSValue* v) {
		std::ostringstream oss;
		oss << *v;
		return oss.str();
	}

	template <class T1, class T2>
	bool equals(const T1 v1, const T2 v2) {
		return v1 == v2;
	}

	template <>
	inline bool equals(double v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(ls::LSValue* v1, double v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(int v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(ls::LSValue* v1, int v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(ls::LSValue* v1, ls::LSValue* v2) {
		return v1->operator == (*v2);
	}

	template <class T> void release(T) {}
	template <> inline void release(LSValue* v) {
		LSValue::delete_temporary(v);
	}
	template <> inline void release(const LSValue* v) {
		LSValue::delete_temporary(v);
	}

	template <class T> void unref(T) {}
	template <> inline void unref(LSValue* v) {
		LSValue::delete_ref(v);
	}
	template <> inline void unref(const LSValue* v) {
		LSValue::delete_ref((LSValue*) v);
	}

	template <class T> T oneref(T v) { return v; }
	template <> inline LSValue* oneref(LSValue* v) {
		v->refs = 1;
		return v;
	}
	template <> inline const LSValue* oneref(const LSValue* v) {
		((LSValue*) v)->refs = 1;
		return v;
	}

	template <class T> T move(T v) { return v; }
	template <> inline LSValue* move(LSValue* v) {
		return v->move();
	}
	template <> inline const LSValue* move(const LSValue* v) {
		return ((LSValue*) v)->move();
	}

	template <class T> T move_inc(T v) { return v; }
	template <> inline LSValue* move_inc(LSValue* v) {
		return v->move_inc();
	}
	template <> inline const LSValue* move_inc(const LSValue* v) {
		return ((LSValue*) v)->move_inc();
	}

	template <class T> T clone(T v) { return v; }
	template <> inline LSValue* clone(LSValue* v) {
		return v->clone();
	}
	template <> inline const LSValue* clone(const LSValue* v) {
		return ((LSValue*) v)->clone();
	}

	template <class T> T clone_inc(T v) { return v; }
	template <> inline LSValue* clone_inc(LSValue* v) {
		return v->clone_inc();
	}
	template <> inline const LSValue* clone_inc(const LSValue* v) {
		return ((LSValue*) v)->clone_inc();
	}

	template <class R, class T> R convert(T v);
	template <class R, R> R convert(R v) { return v; }

	template <> inline int convert(int v) { return v; }
	template <> inline double convert(double v) { return v; }
	template <> inline LSValue* convert(LSValue* v) { return v; }

	template <> inline int convert(double v) { return v; }
	template <> inline double convert(int v) { return v; }

	template <> inline LSValue* convert(int v) {
		return LSValue::get(v);
	}
	template <> inline LSValue* convert(double v) {
		return LSValue::get(v);
	}
	template <> inline int convert(LSValue* v) {
		return v->integer();
	}
	template <> inline double convert(LSValue* v) {
		return v->real();
	}
}

namespace std {
	std::string to_string(ls::LSValue* value);
	std::ostream& operator << (std::ostream&, const ls::LSValue*);
}

#endif
