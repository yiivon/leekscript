#include <sstream>
#include <algorithm>
#include "LSString.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

LSValue* LSString::string_class(new LSClass("String"));

LSString::LSString() {}
LSString::LSString(const char value) : value(string(1, value)) {}
LSString::LSString(const char* value) : value(value) {}
LSString::LSString(std::string value) : value(value) {}
LSString::LSString(JsonValue& json) : value(json.toString()) {}

LSString::~LSString() {}

bool LSString::isTrue() const {
	return value.size() > 0;
}

LSValue* LSString::operator - () const {
	return this->clone();
}

LSValue* LSString::operator ! () const {
	return LSBoolean::get(value.size() == 0);
}

LSValue* LSString::operator ~ () const {
	string copy = value;
	reverse(copy.begin(), copy.end());
	return new LSString(copy);
}

LSValue* LSString::operator ++ () {
	return this;
}
LSValue* LSString::operator ++ (int) {
	return this;
}

LSValue* LSString::operator -- () {
	return this;
}
LSValue* LSString::operator -- (int) {
	return this;
}

LSValue* LSString::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSString::operator + (const LSNull*) const {
	return new LSString(this->value + "null");
}
LSValue* LSString::operator + (const LSBoolean* boolean) const {
	return new LSString(this->value + (boolean->value ? "true" : "false"));
}
LSValue* LSString::operator + (const LSNumber* value) const {
	return new LSString(this->value + value->toString());
}
LSValue* LSString::operator + (const LSString* string) const {
	return new LSString(this->value + string->value);
}
LSValue* LSString::operator + (const LSArray*) const {
	return new LSString(this->value + "<array>");
}
LSValue* LSString::operator + (const LSObject* ) const {
	return new LSString(this->value + "<object>");
}
LSValue* LSString::operator + (const LSFunction*) const {
	return new LSString(this->value + "<function>");
}
LSValue* LSString::operator + (const LSClass*) const {
	return new LSString(this->value + "<class>");
}

LSValue* LSString::operator += (LSValue* value) const {
	return value->operator += (this);
}
LSValue* LSString::operator += (const LSNull*) {
	return this;
}
LSValue* LSString::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSNumber*) {
	return this;
}
LSValue* LSString::operator += (const LSString* string) {
	this->value += string->value;
	return this;
}
LSValue* LSString::operator += (const LSArray*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSClass*) {
	return this;
}

LSValue* LSString::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSString::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSNumber* value) const {
	return new LSString(to_string(value->value) + this->value);
}
LSValue* LSString::operator - (const LSString* string) const {
	return new LSString(string->value + this->value);
}
LSValue* LSString::operator - (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSString::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSString::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSString::operator -= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSArray*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSClass*) {
	return this->clone();
}

LSValue* LSString::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSString::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSNumber* value) const {
	string res = "";
	for (int i = 0; i < value->value; ++i) {
		res += this->value;
	}
	return new LSString(res);
}
LSValue* LSString::operator * (const LSString* string) const {
	return new LSString(string->value);
}
LSValue* LSString::operator * (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator *= (LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSString::operator *= (const LSNull*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSArray*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSClass*) {
	return this->clone();
}

LSValue* LSString::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSString::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSNumber*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator / (const LSString* s) const {
	LSArray* array = new LSArray();
	if (s->value.size() == 0) {
		for (char c : value) {
			array->push(new LSString(string({c})));
		}
 	} else {
		stringstream ss(value);
		string item;
		while (getline(ss, item, s->value[0])) {
			array->push(new LSString(item));
		}
 	}
	return array;
}

LSValue* LSString::operator / (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator /= (LSValue* value) const {
	return value->operator /= (this);
}
LSValue* LSString::operator /= (const LSNull*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSArray*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSClass*) {
	return this->clone();
}

LSValue* LSString::poww(const LSValue*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::pow_eq(LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSString::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSString::pow_eq(const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSNumber*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSString*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSArray*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSObject*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSFunction*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSClass*) {
	return this->clone();
}

LSValue* LSString::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSString::operator % (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSString::operator %= (const LSNull*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSArray*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSClass*) {
	return this->clone();
}

bool LSString::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSString::operator == (const LSNull*) const {
	return false;
}
bool LSString::operator == (const LSBoolean*) const {
	return false;
}
bool LSString::operator == (const LSNumber*) const {
	return false;
}
bool LSString::operator == (const LSString* v) const {
	return this->value == v->value;
}
bool LSString::operator == (const LSArray*) const {
	return false;
}
bool LSString::operator == (const LSFunction*) const {
	return false;
}
bool LSString::operator == (const LSObject*) const {
	return false;
}
bool LSString::operator == (const LSClass*) const {
	return false;
}

bool LSString::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSString::operator < (const LSNull*) const {
	return false;
}
bool LSString::operator < (const LSBoolean*) const {
	return false;
}
bool LSString::operator < (const LSNumber*) const {
	return false;
}
bool LSString::operator < (const LSString* v) const {
	return this->value < v->value;
}
bool LSString::operator < (const LSArray*) const {
	return true;
}
bool LSString::operator < (const LSObject*) const {
	return true;
}
bool LSString::operator < (const LSFunction*) const {
	return true;
}
bool LSString::operator < (const LSClass*) const {
	return true;
}

bool LSString::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSString::operator > (const LSNull*) const {
	return true;
}
bool LSString::operator > (const LSBoolean*) const {
	return true;
}
bool LSString::operator > (const LSNumber*) const {
	return true;
}
bool LSString::operator > (const LSString* v) const {
	return this->value > v->value;
}
bool LSString::operator > (const LSArray*) const {
	return false;
}
bool LSString::operator > (const LSObject*) const {
	return false;
}
bool LSString::operator > (const LSFunction*) const {
	return false;
}
bool LSString::operator > (const LSClass*) const {
	return false;
}

bool LSString::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSString::operator <= (const LSNull*) const {
	return false;
}
bool LSString::operator <= (const LSBoolean*) const {
	return false;
}
bool LSString::operator <= (const LSNumber*) const {
	return false;
}
bool LSString::operator <= (const LSString* v) const {
	return this->value <= v->value;
}
bool LSString::operator <= (const LSArray*) const {
	return true;
}
bool LSString::operator <= (const LSObject*) const {
	return true;
}
bool LSString::operator <= (const LSFunction*) const {
	return true;
}
bool LSString::operator <= (const LSClass*) const {
	return true;
}

bool LSString::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSString::operator >= (const LSNull*) const {
	return true;
}
bool LSString::operator >= (const LSBoolean*) const {
	return true;
}
bool LSString::operator >= (const LSNumber*) const {
	return true;
}
bool LSString::operator >= (const LSString* v) const {
	return this->value >= v->value;
}
bool LSString::operator >= (const LSArray*) const {
	return false;
}
bool LSString::operator >= (const LSObject*) const {
	return false;
}
bool LSString::operator >= (const LSFunction*) const {
	return false;
}
bool LSString::operator >= (const LSClass*) const {
	return false;
}

bool LSString::in(const LSValue*) const {
	return false;
}

LSValue* LSString::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		return new LSString(value[(int)n->value]);
	}
	return LSNull::null_var;
}
LSValue** LSString::atL(const LSValue* key) {
	return &LSNull::null_var;
}

LSValue* LSString::range(const LSValue* start, const LSValue* end) const {

	if (const LSNumber* start_num = dynamic_cast<const LSNumber*>(start)) {
		if (const LSNumber* end_num = dynamic_cast<const LSNumber*>(end)) {
			return new LSString(value.substr(start_num->value, end_num->value - start_num->value + 1));
		}
	}
	return LSNull::null_var;
}
LSValue* LSString::rangeL(const LSValue* start, const LSValue* end) {
	return this;
}

LSValue* LSString::attr(const LSValue* key) const {
	if (((LSString*) key)->value == "class") {
		return getClass();
	}
	return LSNull::null_var;
}
LSValue** LSString::attrL(const LSValue* key) {
	return &LSNull::null_var;
}

LSValue* LSString::abso() const {
	return LSNumber::get(value.size());
}

std::ostream& LSString::print(std::ostream& os) const {
	os << "'" << value << "'";
	return os;
}
string LSString::json() const {
	return "\"" + value + "\"";
}

LSValue* LSString::clone() const {
	return new LSString(value);
}

std::ostream& operator<<(std::ostream& os, const LSString& obj) {
	os << obj.value;
	return os;
}

LSValue* LSString::getClass() const {
	return LSString::string_class;
}

int LSString::typeID() const {
	return 4;
}

RawType LSString::getRawType() const {
	return RawType::STRING;
}
