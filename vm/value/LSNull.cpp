#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

LSValue* LSNull::null_var = new LSNull();
LSClass* LSNull::null_class = new LSClass("Null");

bool LSNull::isTrue() const {
	return false;
}

LSValue* LSNull::operator - () const {
	return LSNull::null_var;
}

LSValue* LSNull::operator ! () const {
	return LSNull::null_var;
}

LSValue* LSNull::operator ~ () const {
	return LSNull::null_var;
}

LSValue* LSNull::operator ++ () {
	return this;
}
LSValue* LSNull::operator ++ (int) {
	return LSNull::null_var;
}

LSValue* LSNull::operator -- () {
	return this;
}
LSValue* LSNull::operator -- (int) {
	return LSNull::null_var;
}

LSValue* LSNull::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSNull::operator + (const LSNull* boolean) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSBoolean* boolean) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSArray* array) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSObject* boolean) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSFunction* boolean) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator + (const LSClass* boolean) const {
	return LSNull::null_var;
}

LSValue* LSNull::operator += (LSValue* value) const {
	return value->operator += (this);
}
LSValue* LSNull::operator += (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator += (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSNull::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSNull::operator - (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator - (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSNull::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSNull::operator -= (const LSNull* number) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSNumber* number) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator -= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSNull::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSNull::operator * (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator * (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSNull::operator *= (LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSNull::operator *= (const LSNull* number) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSBoolean* number) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSNumber* number) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator *= (const LSClass* number) {
	return LSNull::null_var;
}

LSValue* LSNull::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSNull::operator / (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator / (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSNull::operator /= (LSValue* value) const {
	return value->operator /= (this);
}
LSValue* LSNull::operator /= (const LSNull* number) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSNull::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSNull::poww(const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::poww(const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSNull::pow_eq(LSValue* value) const {
	return value->pow_eq(this);
}
LSValue* LSNull::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSString*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSNull::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSNull::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSNull::operator % (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSNull::operator % (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSNull::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSNull::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSNull::operator %= (const LSClass*) {
	return LSNull::null_var;
}

bool LSNull::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSNull::operator == (const LSNull*) const {
	return true;
}
bool LSNull::operator == (const LSBoolean*) const {
	return false;
}
bool LSNull::operator == (const LSNumber*) const {
	return false;
}
bool LSNull::operator == (const LSString*) const {
	return false;
}
bool LSNull::operator == (const LSArray*) const {
	return false;
}
bool LSNull::operator == (const LSFunction*) const {
	return false;
}
bool LSNull::operator == (const LSObject*) const {
	return false;
}
bool LSNull::operator == (const LSClass*) const {
	return false;
}

bool LSNull::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSNull::operator < (const LSNull* v) const {
	return false;
}
bool LSNull::operator < (const LSBoolean* v) const {
	return true;
}
bool LSNull::operator < (const LSNumber* v) const {
	return true;
}
bool LSNull::operator < (const LSString* v) const {
	return true;
}
bool LSNull::operator < (const LSArray* v) const {
	return true;
}
bool LSNull::operator < (const LSFunction* v) const {
	return true;
}
bool LSNull::operator < (const LSObject* v) const {
	return true;
}
bool LSNull::operator < (const LSClass* v) const {
	return true;
}

bool LSNull::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSNull::operator > (const LSNull* v) const {
	return false;
}
bool LSNull::operator > (const LSBoolean* v) const {
	return false;
}
bool LSNull::operator > (const LSNumber* v) const {
	return false;
}
bool LSNull::operator > (const LSString* v) const {
	return false;
}
bool LSNull::operator > (const LSArray* v) const {
	return false;
}
bool LSNull::operator > (const LSFunction* v) const {
	return false;
}
bool LSNull::operator > (const LSObject* v) const {
	return false;
}
bool LSNull::operator > (const LSClass* v) const {
	return false;
}

bool LSNull::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSNull::operator <= (const LSNull* v) const {
	return true;
}
bool LSNull::operator <= (const LSBoolean* v) const {
	return true;
}
bool LSNull::operator <= (const LSNumber* v) const {
	return true;
}
bool LSNull::operator <= (const LSString* v) const {
	return true;
}
bool LSNull::operator <= (const LSArray* v) const {
	return true;
}
bool LSNull::operator <= (const LSFunction* v) const {
	return true;
}
bool LSNull::operator <= (const LSObject* v) const {
	return true;
}
bool LSNull::operator <= (const LSClass* v) const {
	return true;
}

bool LSNull::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSNull::operator >= (const LSNull* v) const {
	return true;
}
bool LSNull::operator >= (const LSBoolean* v) const {
	return false;
}
bool LSNull::operator >= (const LSNumber* v) const {
	return false;
}
bool LSNull::operator >= (const LSString* v) const {
	return false;
}
bool LSNull::operator >= (const LSArray* v) const {
	return false;
}
bool LSNull::operator >= (const LSFunction* v) const {
	return false;
}
bool LSNull::operator >= (const LSObject* v) const {
	return false;
}
bool LSNull::operator >= (const LSClass* v) const {
	return false;
}

bool LSNull::in(const LSValue* v) const {
	return false;
}

LSValue* LSNull::at(const LSValue* value) const {
	return LSNull::null_var;
}
LSValue** LSNull::atL(const LSValue* value) {
	return &LSNull::null_var;
}

LSValue* LSNull::range(const LSValue* start, const LSValue* end) const {
	return this->clone();
}
LSValue* LSNull::rangeL(const LSValue* start, const LSValue* end) {
	return this;
}

LSValue* LSNull::attr(const LSValue* key) const {
	if (((LSString*) key)->value == "class") {
		return getClass();
	}
	return LSNull::null_var;
}
LSValue** LSNull::attrL(const LSValue* key) {
	return &LSNull::null_var;
}

LSValue* LSNull::abso() const {
	return LSNumber::get(0);
}

LSValue* LSNull::clone() const {
	return LSNull::null_var;
}

std::ostream& LSNull::print(std::ostream& os) const {
	os << "null";
	return os;
}
string LSNull::json() const {
	return "\"1|null\"";
}

LSValue* LSNull::getClass() const {
	return LSNull::null_class;
}

int LSNull::typeID() const {
	return 1;
}

RawType LSNull::getRawType() const {
	return RawType::NULLL;
}
