#include "LSBoolean.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSString.hpp"

using namespace std;

LSValue* LSBoolean::boolean_class(new LSClass("Boolean"));
LSBoolean* LSBoolean::false_val(new LSBoolean(false));
LSBoolean* LSBoolean::true_val(new LSBoolean(true));

LSBoolean* LSBoolean::get(bool value) {
	return value ? true_val : false_val;
}

LSBoolean::LSBoolean() : value(true) {}

LSBoolean::LSBoolean(bool value) : value(value) {}

LSBoolean::LSBoolean(JsonValue& json) : value(json.getTag() == JSON_TRUE) {}

LSBoolean::~LSBoolean() {}

bool LSBoolean::isTrue() const {
	return value;
}

LSValue* LSBoolean::operator - () const {
	return (LSValue*) this;
}

LSValue* LSBoolean::operator ! () const {
	return LSBoolean::get(!value);
}

LSValue* LSBoolean::operator ~ () const {
	return LSBoolean::get(!value);
}

LSValue* LSBoolean::operator ++ () {
	return this;
}

LSValue* LSBoolean::operator ++ (int) {
	return this;
}

LSValue* LSBoolean::operator -- () {
	return this;
}
LSValue* LSBoolean::operator -- (int) {
	return this;
}

LSValue* LSBoolean::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSBoolean::operator + (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSBoolean* boolean) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSArray* array) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator += (LSValue* value) const {
	return value->operator += (this);
}
LSValue* LSBoolean::operator += (const LSNull*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSNumber* number) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSString*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSArray*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSObject*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSFunction*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSClass*) {
	return this->clone();
}

LSValue* LSBoolean::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSBoolean::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSBoolean::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator -= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSBoolean::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator * (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator *= (LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSBoolean::operator *= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator *= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSBoolean::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator /= (LSValue* value) const {
	return value->operator /= (this);
}
LSValue* LSBoolean::operator /= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSBoolean::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSBoolean::poww(const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSClass* value) const {
	return LSNull::null_var;
}


LSValue* LSBoolean::pow_eq(LSValue* value) const {
	return value->operator /= (this);
}
LSValue* LSBoolean::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSString*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSBoolean::operator % (const LSNull* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSBoolean* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSNumber* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSString* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSArray* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSObject* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSFunction* value) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSClass* value) const {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSBoolean::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator %= (const LSClass*) {
	return LSNull::null_var;
}

bool LSBoolean::operator == (const LSValue* value) const {
	return value->operator == (this);
}
bool LSBoolean::operator == (const LSNull*) const {
	return false;
}
bool LSBoolean::operator == (const LSBoolean* v) const {
	return v->value == this->value;
}
bool LSBoolean::operator == (const LSNumber*) const {
	return false;
}
bool LSBoolean::operator == (const LSString*) const {
	return false;
}
bool LSBoolean::operator == (const LSArray*) const {
	return false;
}
bool LSBoolean::operator == (const LSFunction*) const {
	return false;
}
bool LSBoolean::operator == (const LSObject*) const {
	return false;
}
bool LSBoolean::operator == (const LSClass*) const {
	return false;
}

bool LSBoolean::operator < (const LSValue* value) const {
	return value->operator < (this);
}
bool LSBoolean::operator < (const LSNull*) const {
	return false;
}
bool LSBoolean::operator < (const LSBoolean* v) const {
	return v->value < this->value;
}
bool LSBoolean::operator < (const LSNumber*) const {
	return true;
}
bool LSBoolean::operator < (const LSString*) const {
	return true;
}
bool LSBoolean::operator < (const LSArray*) const {
	return true;
}
bool LSBoolean::operator < (const LSFunction*) const {
	return true;
}
bool LSBoolean::operator < (const LSObject*) const {
	return true;
}
bool LSBoolean::operator < (const LSClass*) const {
	return true;
}

bool LSBoolean::operator > (const LSValue* value) const {
	return value->operator > (this);
}
bool LSBoolean::operator > (const LSNull*) const {
	return true;
}
bool LSBoolean::operator > (const LSBoolean* v) const {
	return v->value > this->value;
}
bool LSBoolean::operator > (const LSNumber*) const {
	return false;
}
bool LSBoolean::operator > (const LSString*) const {
	return false;
}
bool LSBoolean::operator > (const LSArray*) const {
	return false;
}
bool LSBoolean::operator > (const LSFunction*) const {
	return false;
}
bool LSBoolean::operator > (const LSObject*) const {
	return false;
}
bool LSBoolean::operator > (const LSClass*) const {
	return false;
}

bool LSBoolean::operator <= (const LSValue* value) const {
	return value->operator <= (this);
}
bool LSBoolean::operator <= (const LSNull*) const {
	return false;
}
bool LSBoolean::operator <= (const LSBoolean* v) const {
	return v->value <= this->value;
}
bool LSBoolean::operator <= (const LSNumber*) const {
	return true;
}
bool LSBoolean::operator <= (const LSString*) const {
	return true;
}
bool LSBoolean::operator <= (const LSArray*) const {
	return true;
}
bool LSBoolean::operator <= (const LSFunction*) const {
	return true;
}
bool LSBoolean::operator <= (const LSObject*) const {
	return true;
}
bool LSBoolean::operator <= (const LSClass*) const {
	return true;
}

bool LSBoolean::operator >= (const LSValue* value) const {
	return value->operator >= (this);
}
bool LSBoolean::operator >= (const LSNull*) const {
	return true;
}
bool LSBoolean::operator >= (const LSBoolean* v) const {
	return v->value >= this->value;
}
bool LSBoolean::operator >= (const LSNumber*) const {
	return false;
}
bool LSBoolean::operator >= (const LSString*) const {
	return false;
}
bool LSBoolean::operator >= (const LSArray*) const {
	return false;
}
bool LSBoolean::operator >= (const LSFunction*) const {
	return false;
}
bool LSBoolean::operator >= (const LSObject*) const {
	return false;
}
bool LSBoolean::operator >= (const LSClass*) const {
	return false;
}

bool LSBoolean::in(const LSValue* key) const {
	return key->operator == (this);
}

LSValue* LSBoolean::at(const LSValue* value) const {
	return LSNull::null_var;
}
LSValue** LSBoolean::atL(const LSValue* value) {
	return &LSNull::null_var;
}

LSValue* LSBoolean::range(const LSValue* start, const LSValue* end) const {
	return this->clone();
}

LSValue* LSBoolean::rangeL(const LSValue* start, const LSValue* end) {
	return this;
}

LSValue* LSBoolean::attr(const LSValue* key) const {
	if (((LSString*) key)->value == "class") {
		return getClass();
	}
	return LSNull::null_var;
}
LSValue** LSBoolean::attrL(const LSValue* key) {
	return &LSNull::null_var;
}

LSValue* LSBoolean::abso() const {
	return (LSValue*) this;
}

LSValue* LSBoolean::clone() const {
	return (LSValue*) this;
}

std::ostream& LSBoolean::print(std::ostream& os) const {
	os << (value ? "true" : "false");
	return os;
}

string LSBoolean::json() const {
	return value ? "true" : "false";
}

LSValue* LSBoolean::getClass() const {
	return LSBoolean::boolean_class;
}

int LSBoolean::typeID() const {
	return 2;
}

RawType LSBoolean::getRawType() const {
	return RawType::BOOLEAN;
}
