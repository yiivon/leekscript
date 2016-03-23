#include "LSClass.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"

using namespace std;

LSValue* LSClass::class_class(new LSClass("Class"));

LSClass::LSClass() : name("?") {
	parent = nullptr;
}

LSClass::LSClass(string name) : name(name) {
	parent = nullptr;
}

LSClass::LSClass(JsonValue&) {
	parent = nullptr;
	// TODO
}

LSClass::~LSClass() {}

void LSClass::addMethod(string name, LSValue* method) {
	methods.insert(pair<string, LSValue*>(name, method));
}

void LSClass::addStaticField(string name, LSValue* value) {
	static_fields.insert(pair<string, LSValue*>(name, value));
}

LSValue* LSClass::getMethod(string name) {
	try {
		return methods.at(name);
	} catch (exception& e) {
		return LSNull::null_var;
	}
}

bool LSClass::isTrue() const {
	return false;
}

LSValue* LSClass::operator - () const {
	return (LSValue*) this;
}

LSValue* LSClass::operator ! () const {
	return (LSValue*) this;
}

LSValue* LSClass::operator ~ () const {
	return (LSValue*) this;
}

LSValue* LSClass::operator ++ () {
	return this;
}
LSValue* LSClass::operator ++ (int) {
	return this;
}

LSValue* LSClass::operator -- () {
	return this;
}
LSValue* LSClass::operator -- (int) {
	return this;
}

LSValue* LSClass::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSClass::operator + (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator + (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSClass::operator += (LSValue* value) const {
	return value->operator += (this);
}
LSValue* LSClass::operator += (const LSNull*) {
	return this;
}
LSValue* LSClass::operator += (const LSNumber*) {
	return this;
}
LSValue* LSClass::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSClass::operator += (const LSString*) {
	return this->clone();
}
LSValue* LSClass::operator += (const LSArray*) {
	return this->clone();
}
LSValue* LSClass::operator += (const LSObject*) {
	return this->clone();
}
LSValue* LSClass::operator += (const LSFunction*) {
	return this->clone();
}
LSValue* LSClass::operator += (const LSClass*) {
	return this->clone();
}

LSValue* LSClass::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSClass::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSClass::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSClass::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator -= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSClass::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSClass::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator * (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSClass::operator *= (LSValue* value) const {
	return value->operator *= (this);
}
LSValue* LSClass::operator *= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator *= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSClass::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSClass::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSClass::operator /= (LSValue* value) const {
	return value->operator /= (this);
}
LSValue* LSClass::operator /= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSClass::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSClass::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSClass::poww(const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSClass::pow_eq(LSValue* value) const {
	return value->pow_eq(this);
}
LSValue* LSClass::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSString*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSClass::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSClass::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSClass::operator % (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSClass::operator % (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSClass::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSClass::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSClass::operator %= (const LSClass*) {
	return LSNull::null_var;
}

bool LSClass::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSClass::operator == (const LSNull*) const {
	return false;
}
bool LSClass::operator == (const LSBoolean*) const {
	return false;
}
bool LSClass::operator == (const LSNumber*) const {
	return false;
}
bool LSClass::operator == (const LSString*) const {
	return false;
}
bool LSClass::operator == (const LSArray*) const {
	return false;
}
bool LSClass::operator == (const LSFunction*) const {
	return false;
}
bool LSClass::operator == (const LSObject*) const {
	return false;
}
bool LSClass::operator == (const LSClass*) const {
	return false;
}

bool LSClass::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSClass::operator < (const LSNull*) const {
	return false;
}
bool LSClass::operator < (const LSBoolean*) const {
	return false;
}
bool LSClass::operator < (const LSNumber*) const {
	return false;
}
bool LSClass::operator < (const LSString*) const {
	return false;
}
bool LSClass::operator < (const LSArray*) const {
	return false;
}
bool LSClass::operator < (const LSObject*) const {
	return false;
}
bool LSClass::operator < (const LSFunction*) const {
	return false;
}
bool LSClass::operator < (const LSClass*) const {
	return false;
}

bool LSClass::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSClass::operator > (const LSNull*) const {
	return true;
}
bool LSClass::operator > (const LSBoolean*) const {
	return true;
}
bool LSClass::operator > (const LSNumber*) const {
	return true;
}
bool LSClass::operator > (const LSString*) const {
	return true;
}
bool LSClass::operator > (const LSArray*) const {
	return true;
}
bool LSClass::operator > (const LSObject*) const {
	return true;
}
bool LSClass::operator > (const LSFunction*) const {
	return true;
}
bool LSClass::operator > (const LSClass*) const {
	return true;
}

bool LSClass::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSClass::operator <= (const LSNull*) const {
	return false;
}
bool LSClass::operator <= (const LSBoolean*) const {
	return false;
}
bool LSClass::operator <= (const LSNumber*) const {
	return false;
}
bool LSClass::operator <= (const LSString*) const {
	return false;
}
bool LSClass::operator <= (const LSArray*) const {
	return false;
}
bool LSClass::operator <= (const LSObject*) const {
	return false;
}
bool LSClass::operator <= (const LSFunction*) const {
	return false;
}
bool LSClass::operator <= (const LSClass*) const {
	return false;
}

bool LSClass::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSClass::operator >= (const LSNull*) const {
	return true;
}
bool LSClass::operator >= (const LSBoolean*) const {
	return true;
}
bool LSClass::operator >= (const LSNumber*) const {
	return true;
}
bool LSClass::operator >= (const LSString*) const {
	return true;
}
bool LSClass::operator >= (const LSArray*) const {
	return true;
}
bool LSClass::operator >= (const LSObject*) const {
	return true;
}
bool LSClass::operator >= (const LSFunction*) const {
	return true;
}
bool LSClass::operator >= (const LSClass*) const {
	return true;
}

bool LSClass::in(const LSValue*) const {
	return false;
}

LSValue* LSClass::at(const LSValue*) const {
	return LSNull::null_var;
}

LSValue** LSClass::atL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSClass::range(const LSValue*, const LSValue*) const {
	return this->clone();
}

LSValue* LSClass::rangeL(const LSValue*, const LSValue*) {
	return this;
}

LSValue* LSClass::attr(const LSValue* key) const {
	if (((LSString*) key)->value == "class") {
		return getClass();
	}
	if (key->operator == (new LSString("name"))) {
		return new LSString(name);
	}
	try {
		return static_fields.at(((LSString*) key)->value);
	} catch (exception& e) {}
	return LSNull::null_var;
}

LSValue** LSClass::attrL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSClass::abso() const {
	return LSNumber::get(0);
}

LSValue* LSClass::clone() const {
	return new LSClass(name);
}

std::ostream& LSClass::print(std::ostream& os) const {
	os << "<class " << name << ">";
	return os;
}

string LSClass::json() const {
	return "class";
}

LSValue* LSClass::getClass() const {
	return LSClass::class_class;
}

int LSClass::typeID() const {
	return 8;
}

RawType LSClass::getRawType() const {
	return RawType::CLASS;
}
