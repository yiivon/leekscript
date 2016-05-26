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
LSValue* LSBoolean::operator + (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator + (const LSArray<int>*) const {
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

LSValue* LSBoolean::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSBoolean::operator += (const LSNull*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSNumber*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSString*) {
	return this->clone();
}
LSValue* LSBoolean::operator += (const LSArray<LSValue*>*) {
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
LSValue* LSBoolean::operator - (const LSArray<LSValue*>*) const {
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

LSValue* LSBoolean::operator -= (LSValue* value) {
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
LSValue* LSBoolean::operator -= (const LSArray<LSValue*>*) {
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
LSValue* LSBoolean::operator * (const LSArray<LSValue*>*) const {
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

LSValue* LSBoolean::operator *= (LSValue* value) {
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
LSValue* LSBoolean::operator *= (const LSArray<LSValue*>*) {
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
LSValue* LSBoolean::operator / (const LSArray<LSValue*>*) const {
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

LSValue* LSBoolean::operator /= (LSValue* value) {
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
LSValue* LSBoolean::operator /= (const LSArray<LSValue*>*) {
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
LSValue* LSBoolean::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::poww(const LSClass*) const {
	return LSNull::null_var;
}


LSValue* LSBoolean::pow_eq(LSValue* value) {
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
LSValue* LSBoolean::pow_eq(const LSArray<LSValue*>*) {
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
LSValue* LSBoolean::operator % (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSBoolean::operator % (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSBoolean::operator %= (LSValue* value) {
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
LSValue* LSBoolean::operator %= (const LSArray<LSValue*>*) {
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
bool LSBoolean::operator == (const LSArray<LSValue*>*) const {
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
bool LSBoolean::operator < (const LSArray<LSValue*>*) const {
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
bool LSBoolean::operator > (const LSArray<LSValue*>*) const {
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
bool LSBoolean::operator <= (const LSArray<LSValue*>*) const {
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
bool LSBoolean::operator >= (const LSArray<LSValue*>*) const {
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

LSValue* LSBoolean::at(const LSValue*) const {
	return LSNull::null_var;
}
LSValue** LSBoolean::atL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSBoolean::range(int, int) const {
	return this->clone();
}

LSValue* LSBoolean::rangeL(int, int) {
	return this;
}

LSValue* LSBoolean::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::null_var;
}
LSValue** LSBoolean::attrL(const LSValue*) {
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

const BaseRawType* LSBoolean::getRawType() const {
	return RawType::BOOLEAN;
}
