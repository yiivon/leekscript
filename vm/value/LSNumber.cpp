#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include <math.h>
#include "LSBoolean.hpp"

using namespace std;

LSClass* LSNumber::number_class = new LSClass("Number");

LSNumber* LSNumber::cache[CACHE_HIGH - CACHE_LOW + 1];

void LSNumber::build_cache() {
	for (int i = CACHE_LOW; i <= CACHE_HIGH; ++i) {
		cache[-CACHE_LOW + i] = new LSNumber(i);
	}
}

LSNumber* LSNumber::get(NUMBER_TYPE i) {
#if USE_CACHE
	if ((i == (int) i) and i >= CACHE_LOW and i <= CACHE_HIGH) {
		return cache[(int) (-CACHE_LOW + i)];
	}
#endif
	return new LSNumber(i);
}

LSNumber::LSNumber() : value(0) {}

LSNumber::LSNumber(NUMBER_TYPE value) : value(value) {}

LSNumber::LSNumber(JsonValue& json) : value(json.toNumber()) {}

LSNumber::~LSNumber() {}

bool LSNumber::isTrue() const {
	return value != 0;
}

LSValue* LSNumber::operator - () const {
	return LSNumber::get(-value);
}

LSValue* LSNumber::operator ! () const {
	return LSBoolean::get(value == 0);
}

LSValue* LSNumber::operator ~ () const {
	return LSNumber::get(~(int)value);
}

LSValue* LSNumber::operator ++ () {
#if !USE_CACHE
	++value;
#endif
	return this;
}
LSValue* LSNumber::operator ++ (int) {
	NUMBER_TYPE old = value;
#if !USE_CACHE
	++value;
#endif
	return LSNumber::get(old);
}

LSValue* LSNumber::operator -- () {
#if !USE_CACHE
	--value;
#endif
	return this;
}
LSValue* LSNumber::operator -- (int) {
	NUMBER_TYPE old = value;
#if !USE_CACHE
	--value;
#endif
	return LSNumber::get(old);
}

LSValue* LSNumber::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSNumber::operator + (const LSNull* nulll) const {
	return this->clone();
}
LSValue* LSNumber::operator + (const LSBoolean* boolean) const {
	return LSNumber::get(value + boolean->value);
}
LSValue* LSNumber::operator + (const LSString* string) const {
	return new LSString(toString() + string->value);
}
LSValue* LSNumber::operator + (const LSNumber* number) const {
	return LSNumber::get(this->value + number->value);
}
LSValue* LSNumber::operator + (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator + (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator + (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator + (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSNumber::operator += (LSValue* value) const {
	return value->operator += (this);
}
LSValue* LSNumber::operator += (const LSNull*) {
	return this;
}
LSValue* LSNumber::operator += (const LSNumber* number) {
#if !USE_CACHE
	value += number->value;
#endif
	return this;
}
LSValue* LSNumber::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSNumber::operator += (const LSString*) {
	return this->clone();
}
LSValue* LSNumber::operator += (const LSArray*) {
	return this->clone();
}
LSValue* LSNumber::operator += (const LSObject*) {
	return this->clone();
}
LSValue* LSNumber::operator += (const LSFunction*) {
	return this->clone();
}
LSValue* LSNumber::operator += (const LSClass*) {
	return this->clone();
}

LSValue* LSNumber::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSNumber::operator - (const LSNull*) const {
	return clone();
}
LSValue* LSNumber::operator - (const LSBoolean*) const {
	return clone();
}
LSValue* LSNumber::operator - (const LSNumber* number) const {
	return LSNumber::get(this->value - number->value);
}
LSValue* LSNumber::operator - (const LSString* value) const {
	return new LSString(value->value + to_string(this->value));
}
LSValue* LSNumber::operator - (const LSArray*) const {
	return clone();
}
LSValue* LSNumber::operator - (const LSObject*) const {
	return clone();
}
LSValue* LSNumber::operator - (const LSFunction*) const {
	return clone();
}
LSValue* LSNumber::operator - (const LSClass*) const {
	return clone();
}


LSValue* LSNumber::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSNumber::operator -= (const LSNull*) {
	return this->clone();
}
LSValue* LSNumber::operator -= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSNumber::operator -= (const LSNumber* number) {
#if !USE_CACHE
	value -= number->value;
#endif
	return this;
}
LSValue* LSNumber::operator -= (const LSString*) {
	return this->clone();
}
LSValue* LSNumber::operator -= (const LSArray*) {
	return this->clone();
}
LSValue* LSNumber::operator -= (const LSObject*) {
	return this->clone();
}
LSValue* LSNumber::operator -= (const LSFunction*) {
	return this->clone();
}
LSValue* LSNumber::operator -= (const LSClass*) {
	return this->clone();
}

LSValue* LSNumber::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSNumber::operator * (const LSNull*) const {
	return this->clone();
}
LSValue* LSNumber::operator * (const LSBoolean*) const {
	return this->clone();
}
LSValue* LSNumber::operator * (const LSNumber* number) const {
	return LSNumber::get(this->value * number->value);
}
LSValue* LSNumber::operator * (const LSString* value) const {
	return new LSString(value->value + to_string(this->value));
}
LSValue* LSNumber::operator * (const LSArray*) const {
	return this->clone();
}
LSValue* LSNumber::operator * (const LSObject*) const {
	return this->clone();
}
LSValue* LSNumber::operator * (const LSFunction*) const {
	return this->clone();
}
LSValue* LSNumber::operator * (const LSClass*) const {
	return this->clone();
}

LSValue* LSNumber::operator *= (LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSNumber::operator *= (const LSNull*) {
	return LSNumber::get(0);
}
LSValue* LSNumber::operator *= (const LSBoolean* boolean) {
	return LSNumber::get(value * boolean->value);
}
LSValue* LSNumber::operator *= (const LSNumber* number) {
#if !USE_CACHE
	value *= number->value;
#endif
	return this;
}
LSValue* LSNumber::operator *= (const LSString*) {
	return this->clone();
}
LSValue* LSNumber::operator *= (const LSArray*) {
	return this->clone();
}
LSValue* LSNumber::operator *= (const LSObject*) {
	return this->clone();
}
LSValue* LSNumber::operator *= (const LSFunction*) {
	return this->clone();
}
LSValue* LSNumber::operator *= (const LSClass*) {
	return this->clone();
}

LSValue* LSNumber::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSNumber::operator / (const LSNull*) const {
	return this->clone();
}
LSValue* LSNumber::operator / (const LSBoolean*) const {
	return this->clone();
}
LSValue* LSNumber::operator / (const LSNumber* number) const {
	return LSNumber::get(this->value / number->value);
}
LSValue* LSNumber::operator / (const LSString* value) const {
	return new LSString(value->value + to_string(this->value));
}
LSValue* LSNumber::operator / (const LSArray*) const {
	return this->clone();
}
LSValue* LSNumber::operator / (const LSObject*) const {
	return this->clone();
}
LSValue* LSNumber::operator / (const LSFunction*) const {
	return this->clone();
}
LSValue* LSNumber::operator / (const LSClass*) const {
	return this->clone();
}
LSValue* LSNumber::operator /= (const LSNull*) {
	return this->clone();
}
LSValue* LSNumber::operator /= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSNumber::operator /= (LSValue* value) const {
	return value->operator /= (this);
}

LSValue* LSNumber::operator /= (const LSNumber* number) {
#if !USE_CACHE
	value /= number->value;
#endif
	return this;
}
LSValue* LSNumber::operator /= (const LSString*) {
	return this->clone();
}
LSValue* LSNumber::operator /= (const LSArray*) {
	return this->clone();
}
LSValue* LSNumber::operator /= (const LSObject*) {
	return this->clone();
}
LSValue* LSNumber::operator /= (const LSFunction*) {
	return this->clone();
}
LSValue* LSNumber::operator /= (const LSClass*) {
	return this->clone();
}

LSValue* LSNumber::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSNumber::poww(const LSNull* value) const {
	return this->clone();
}
LSValue* LSNumber::poww(const LSBoolean* value) const {
	return this->clone();
}
LSValue* LSNumber::poww(const LSNumber* value) const {
	return LSNumber::get((NUMBER_TYPE) pow(this->value, value->value));
}
LSValue* LSNumber::poww(const LSString* value) const {
	return this->clone();
}
LSValue* LSNumber::poww(const LSArray* value) const {
	return this->clone();
}
LSValue* LSNumber::poww(const LSObject* value) const {
	return this->clone();
}
LSValue* LSNumber::poww(const LSFunction* value) const {
	return this->clone();
}
LSValue* LSNumber::poww(const LSClass* value) const {
	return this->clone();
}

LSValue* LSNumber::pow_eq(LSValue* value) const {
	return value->pow_eq(this);
}
LSValue* LSNumber::pow_eq(const LSNull* value) {
	return this->clone();
}
LSValue* LSNumber::pow_eq(const LSBoolean* value) {
	return value->pow_eq(this);
}
LSValue* LSNumber::pow_eq(const LSNumber* number) {
#if !USE_CACHE
	value = pow(value, number->value);
#endif
	return this;
}
LSValue* LSNumber::pow_eq(const LSString*) {
	return this->clone();
}
LSValue* LSNumber::pow_eq(const LSArray*) {
	return this->clone();
}
LSValue* LSNumber::pow_eq(const LSObject*) {
	return this->clone();
}
LSValue* LSNumber::pow_eq(const LSFunction*) {
	return this->clone();
}
LSValue* LSNumber::pow_eq(const LSClass*) {
	return this->clone();
}

LSValue* LSNumber::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSNumber::operator % (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator % (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator % (const LSNumber* value) const {
	return LSNumber::get(fmod(this->value, value->value));
}
LSValue* LSNumber::operator % (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator % (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator % (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator % (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSNumber::operator % (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSNumber::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSNumber::operator %= (const LSNull*) {
	return this->clone();
}
LSValue* LSNumber::operator %= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSNumber::operator %= (const LSNumber* number) {
#if !USE_CACHE
	value = fmod(value, number->value);
#endif
	return this;
}
LSValue* LSNumber::operator %= (const LSString*) {
	return this->clone();
}
LSValue* LSNumber::operator %= (const LSArray*) {
	return this->clone();
}
LSValue* LSNumber::operator %= (const LSObject*) {
	return this->clone();
}
LSValue* LSNumber::operator %= (const LSFunction*) {
	return this->clone();
}
LSValue* LSNumber::operator %= (const LSClass*) {
	return this->clone();
}

bool LSNumber::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSNumber::operator == (const LSNull* v) const {
	return false;
}
bool LSNumber::operator == (const LSBoolean* v) const {
	return false;
}
bool LSNumber::operator == (const LSNumber* v) const {
	return this->value == v->value;
}
bool LSNumber::operator == (const LSString*) const {
	return false;
}
bool LSNumber::operator == (const LSArray* v) const {
	return false;
}
bool LSNumber::operator == (const LSFunction* v) const {
	return false;
}
bool LSNumber::operator == (const LSObject* v) const {
	return false;
}
bool LSNumber::operator == (const LSClass* v) const {
	return false;
}


bool LSNumber::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSNumber::operator < (const LSNull* v) const {
	return false;
}
bool LSNumber::operator < (const LSBoolean* v) const {
	return false;
}
bool LSNumber::operator < (const LSNumber* v) const {
	return this->value < v->value;
}
bool LSNumber::operator < (const LSString*) const {
	return true;
}
bool LSNumber::operator < (const LSArray*) const {
	return true;
}
bool LSNumber::operator < (const LSObject* v) const {
	return true;
}
bool LSNumber::operator < (const LSFunction* v) const {
	return true;
}
bool LSNumber::operator < (const LSClass* v) const {
	return true;
}


bool LSNumber::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSNumber::operator > (const LSNull* v) const {
	return true;
}
bool LSNumber::operator > (const LSBoolean* v) const {
	return true;
}
bool LSNumber::operator > (const LSNumber* v) const {
	return this->value > v->value;
}
bool LSNumber::operator > (const LSString*) const {
	return false;
}
bool LSNumber::operator > (const LSArray*) const {
	return false;
}
bool LSNumber::operator > (const LSObject* v) const {
	return false;
}
bool LSNumber::operator > (const LSFunction* v) const {
	return false;
}
bool LSNumber::operator > (const LSClass* v) const {
	return false;
}

bool LSNumber::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSNumber::operator <= (const LSNull* v) const {
	return false;
}
bool LSNumber::operator <= (const LSBoolean* v) const {
	return false;
}
bool LSNumber::operator <= (const LSNumber* v) const {
	return this->value <= v->value;
}
bool LSNumber::operator <= (const LSString*) const {
	return true;
}
bool LSNumber::operator <= (const LSArray*) const {
	return true;
}
bool LSNumber::operator <= (const LSObject* v) const {
	return true;
}
bool LSNumber::operator <= (const LSFunction* v) const {
	return true;
}
bool LSNumber::operator <= (const LSClass* v) const {
	return true;
}


bool LSNumber::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSNumber::operator >= (const LSNull* v) const {
	return true;
}
bool LSNumber::operator >= (const LSBoolean* v) const {
	return true;
}
bool LSNumber::operator >= (const LSNumber* v) const {
	return this->value >= v->value;
}
bool LSNumber::operator >= (const LSString*) const {
	return false;
}
bool LSNumber::operator >= (const LSArray*) const {
	return false;
}
bool LSNumber::operator >= (const LSObject* v) const {
	return false;
}
bool LSNumber::operator >= (const LSFunction* v) const {
	return false;
}
bool LSNumber::operator >= (const LSClass* v) const {
	return false;
}

bool LSNumber::in(const LSValue*) const {
	return false;
}

LSValue* LSNumber::at(const LSValue* value) const {
	return LSNull::null_var;
}
LSValue** LSNumber::atL(const LSValue* value) {
	return &LSNull::null_var;
}

LSValue* LSNumber::range(const LSValue* start, const LSValue* end) const {
	return this->clone();
}
LSValue* LSNumber::rangeL(const LSValue* start, const LSValue* end) {
	return this;
}

LSValue* LSNumber::attr(const LSValue* key) const {
	if (((LSString*) key)->value == "class") {
		return getClass();
	}
	//	if (key->operator == (new LSString("abs"))) {
	//		return new LSFunction([=](Context& ctx, vector<LSValue*> __args) -> LSValue* {
	//			return new LSNumber(abs((int)this->value));
	//		});
	//	}
	return LSNull::null_var;
}
LSValue** LSNumber::attrL(const LSValue* key) {
	return &LSNull::null_var;
}

LSValue* LSNumber::abso() const {
	return LSNumber::get(abs((int) value));
}

LSValue* LSNumber::clone() const {
	return LSNumber::get(this->value);
}

bool LSNumber::isInteger() const {
	return value == (int)value;
}

void append_dbl2str(std::string &s, double d) {
	size_t len = snprintf(0, 0, "%.10f", d);
	size_t oldsize = s.size();
	s.resize(oldsize + len + 1);
	// technically non-portable
	snprintf(&s[oldsize], len+1, "%.10f", d);
	// remove nul terminator
	s.pop_back();
	// remove trailing zeros
	s.erase(s.find_last_not_of('0') + 1, string::npos);
	// remove trailing point
	if (s.back() == '.') {
		s.pop_back();
	}
}

string LSNumber::toString() const {
	if (isInteger()) return to_string((int)value);
	string s;
	append_dbl2str(s, value);
	return s;
}
string LSNumber::json() const {
	return toString();
}

std::ostream& LSNumber::print(std::ostream& os) const {
	os << toString();
	return os;
}

LSValue* LSNumber::getClass() const {
	return LSNumber::number_class;
}

int LSNumber::typeID() const {
	return 3;
}

RawType LSNumber::getRawType() const {
	return RawType::INTEGER;
}
