#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

LSClass* LSFunction::function_class = new LSClass("Function");

LSFunction::LSFunction(void* function) {
	this->function = function;
}
LSFunction::LSFunction(JsonValue& data) {

}

bool LSFunction::isTrue() const {
	return true;
}

LSValue* LSFunction::operator - () const {
	return this->clone();
}

LSValue* LSFunction::operator ! () const {
	return this->clone();
}

LSValue* LSFunction::operator ~ () const {
	return this->clone();
}

LSValue* LSFunction::operator ++ () {
	return this;
}
LSValue* LSFunction::operator ++ (int) {
	return this;
}

LSValue* LSFunction::operator -- () {
	return this;
}
LSValue* LSFunction::operator -- (int) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSFunction::operator + (const LSNull* boolean) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSBoolean* boolean) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSNumber* number) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSString* number) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSArray* array) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSObject* boolean) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSFunction* boolean) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSClass* boolean) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator += (LSValue* value) const {
	return value->operator += (this);
}
LSValue* LSFunction::operator += (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSFunction::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSFunction::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSFunction::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSClass* number) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator *= (LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSFunction::operator *= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSFunction::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator /= (LSValue* value) const {
	return value->operator /= (this);
}
LSValue* LSFunction::operator /= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSFunction::poww(const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSFunction::pow_eq(LSValue* value) const {
	return value->pow_eq(this);
}
LSValue* LSFunction::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSFunction::operator % (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSFunction::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSClass*) {
	return LSNull::null_var;
}


bool LSFunction::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSFunction::operator == (const LSNull*) const {
	return false;
}
bool LSFunction::operator == (const LSBoolean*) const {
	return false;
}
bool LSFunction::operator == (const LSNumber*) const {
	return false;
}
bool LSFunction::operator == (const LSString*) const {
	return false;
}
bool LSFunction::operator == (const LSArray*) const {
	return false;
}
bool LSFunction::operator == (const LSObject*) const {
	return false;
}
bool LSFunction::operator == (const LSFunction*) const {
	return false;
}
bool LSFunction::operator == (const LSClass*) const {
	return false;
}

bool LSFunction::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSFunction::operator < (const LSNull* v) const {
	return false;
}
bool LSFunction::operator < (const LSBoolean* v) const {
	return false;
}
bool LSFunction::operator < (const LSNumber* v) const {
	return false;
}
bool LSFunction::operator < (const LSString* v) const {
	return false;
}
bool LSFunction::operator < (const LSArray* v) const {
	return false;
}
bool LSFunction::operator < (const LSObject* v) const {
	return false;
}
bool LSFunction::operator < (const LSFunction* v) const {
	return false;
}
bool LSFunction::operator < (const LSClass* v) const {
	return true;
}

bool LSFunction::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSFunction::operator > (const LSNull* v) const {
	return true;
}
bool LSFunction::operator > (const LSBoolean* v) const {
	return true;
}
bool LSFunction::operator > (const LSNumber* v) const {
	return true;
}
bool LSFunction::operator > (const LSString* v) const {
	return true;
}
bool LSFunction::operator > (const LSArray* v) const {
	return true;
}
bool LSFunction::operator > (const LSObject* v) const {
	return true;
}
bool LSFunction::operator > (const LSFunction* v) const {
	return true;
}
bool LSFunction::operator > (const LSClass* v) const {
	return false;
}

bool LSFunction::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSFunction::operator <= (const LSNull* v) const {
	return false;
}
bool LSFunction::operator <= (const LSBoolean* v) const {
	return false;
}
bool LSFunction::operator <= (const LSNumber* v) const {
	return false;
}
bool LSFunction::operator <= (const LSString* v) const {
	return false;
}
bool LSFunction::operator <= (const LSArray* v) const {
	return false;
}
bool LSFunction::operator <= (const LSObject* v) const {
	return false;
}
bool LSFunction::operator <= (const LSFunction* v) const {
	return false;
}
bool LSFunction::operator <= (const LSClass* v) const {
	return true;
}

bool LSFunction::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSFunction::operator >= (const LSNull* v) const {
	return true;
}
bool LSFunction::operator >= (const LSBoolean* v) const {
	return true;
}
bool LSFunction::operator >= (const LSNumber* v) const {
	return true;
}
bool LSFunction::operator >= (const LSString* v) const {
	return true;
}
bool LSFunction::operator >= (const LSArray* v) const {
	return true;
}
bool LSFunction::operator >= (const LSObject* v) const {
	return true;
}
bool LSFunction::operator >= (const LSFunction* v) const {
	return true;
}
bool LSFunction::operator >= (const LSClass* v) const {
	return false;
}

bool LSFunction::in(const LSValue*) const {
	return false;
}

LSValue* LSFunction::at(const LSValue* value) const {
	return LSNull::null_var;
}
LSValue** LSFunction::atL(const LSValue* value) {
	return &LSNull::null_var;
}

LSValue* LSFunction::range(const LSValue* start, const LSValue* end) const {
	return this->clone();
}
LSValue* LSFunction::rangeL(const LSValue* start, const LSValue* end) {
	return this;
}

LSValue* LSFunction::attr(const LSValue* key) const {
	if (((LSString*) key)->value == "class") {
		return getClass();
	}
	return LSNull::null_var;
}
LSValue** LSFunction::attrL(const LSValue* key) {
	return &LSNull::null_var;
}

LSValue* LSFunction::abso() const {
	return LSNumber::get(0);
}

LSValue* LSFunction::clone() const {
	return (LSValue*) this;
}

std::ostream& LSFunction::print(std::ostream& os) const {
	os << "<function>";
	return os;
}
string LSFunction::json() const {
	return "\"<function>\"";
}

LSValue* LSFunction::getClass() const {
	return LSFunction::function_class;
}

int LSFunction::typeID() const {
	return 7;
}

RawType LSFunction::getRawType() const {
	return RawType::FUNCTION;
}
