#include "LSBoolean.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSValue* LSBoolean::boolean_class(new LSClass("Boolean"));
LSBoolean* LSBoolean::false_val(new LSBoolean(false));
LSBoolean* LSBoolean::true_val(new LSBoolean(true));

LSBoolean* LSBoolean::get(bool value) {
	return value ? true_val : false_val;
}

LSBoolean::LSBoolean() : LSBoolean(true) {}

LSBoolean::LSBoolean(bool value) : value(value) {
	refs = 1;
	native = true;
}

LSBoolean::LSBoolean(Json& json) : LSBoolean((bool) json) {}

LSBoolean::~LSBoolean() {}

bool LSBoolean::isTrue() const {
	return value;
}

LSValue* LSBoolean::operator ! () const {
	return LSBoolean::get(not value);
}

LSValue* LSBoolean::operator ~ () const {
	return LSBoolean::get(not value);
}

LSValue* LSBoolean::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSBoolean::operator + (const LSNumber*n) const {
	return LSNumber::get(this->value + n->value);
}

LSValue* LSBoolean::operator += (LSValue* value) {
	return value->operator += (this);
}

LSValue* LSBoolean::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSBoolean::operator - (const LSNumber*n) const {
	return LSNumber::get(this->value - n->value);
}

LSValue* LSBoolean::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSBoolean::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSBoolean::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSBoolean::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSBoolean::operator /= (LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSBoolean::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSBoolean::pow_eq(LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSBoolean::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSBoolean::operator %= (LSValue* value) {
	return value->operator %= (this);
}


bool LSBoolean::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSBoolean::operator == (const LSBoolean* v) const {
	return v->value == this->value;
}


bool LSBoolean::operator < (const LSValue* v) const {
	return v->operator < (this);
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
bool LSBoolean::operator < (const LSArray<int>*) const {
	return true;
}
bool LSBoolean::operator < (const LSArray<double>*) const {
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


LSValue* LSBoolean::at(const LSValue*) const {
	return LSNull::get();
}
LSValue** LSBoolean::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSBoolean::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSBoolean::attrL(const LSValue*) {
	return nullptr;
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

}
