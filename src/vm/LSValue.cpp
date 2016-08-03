#include <iostream>
#include "LSValue.hpp"
#include "value/LSNumber.hpp"
#include "value/LSNull.hpp"
#include "value/LSBoolean.hpp"
#include "value/LSArray.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "VM.hpp"

using namespace std;

namespace ls {

int LSValue::obj_count = 0;
int LSValue::obj_deleted = 0;
extern std::map<LSValue*, LSValue*> objs;

LSValue::LSValue() {
//	cout << "LSValue()" << endl;
	native = false;
	obj_count++;
	//objs.insert({this, this});
}

LSValue::~LSValue() {
//	cout << "~LSValue()" << endl;
	obj_deleted++;
	objs.erase(this);
}

std::ostream& operator << (std::ostream& os, LSValue& value) {
	cout << "print LSValue" << endl;
	value.print(os);
	return os;
}

bool LSValue::operator != (const LSValue* value) const {
	return !value->operator == (this);
}

bool LSValue::isInteger() const {
	if (const LSNumber* v = dynamic_cast<const LSNumber*>(this)) {
		return v->isInteger();
	}
	return false;
}

LSValue* get_value(int type, Json& json) {
	switch (type) {
	case 1: return new LSNull();
	case 2: return new LSBoolean(json);
	case 3: return new LSNumber(json);
	case 4: return new LSString(json);
	case 5: return new LSArray<LSValue*>(json);
	case 6: return new LSObject(json);
	case 7: return new LSFunction(json);
	case 8: return new LSClass(json);
	}
	return new LSNull();
}

LSValue* LSValue::parse(Json& json) {

	int type = json["t"];
	Json data = json["v"];
	return get_value(type, data);
}

std::string LSValue::to_json() const {
	return "{\"t\":" + to_string(typeID()) + ",\"v\":" + json() + "}";
}

void LSValue::delete_val(LSValue* value) {

	if (value->native) return;

	if (value == nullptr) return;

//	cout << "LSValue::delete_val ";
//	value->print(cout);
//	cout << " " << value->refs << endl;

	value->refs--;
	if (value->refs <= 0) {
		//cout << "delete LSValue" << endl;
		delete value;
	}
}

LSValue* LSValue::operator - () const { return LSNull::null_var; }
LSValue* LSValue::operator ! () const { return LSNull::null_var; }
LSValue* LSValue::operator ~ () const { return LSNull::null_var; }

LSValue* LSValue::operator ++ () { return LSNull::null_var; }
LSValue* LSValue::operator ++ (int) { return LSNull::null_var; }

LSValue* LSValue::operator -- () { return LSNull::null_var; }
LSValue* LSValue::operator -- (int) { return LSNull::null_var; }

//LSValue* LSValue::operator + (const LSValue*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSNull*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSBoolean*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSNumber*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSString*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSArray<LSValue*>*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSArray<int>*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSArray<double>*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSObject*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSFunction*) const { return LSNull::null_var; }
LSValue* LSValue::operator + (const LSClass*) const { return LSNull::null_var; }

//LSValue* LSValue::operator += (LSValue*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSNull*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSBoolean*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSNumber*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSString*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSArray<LSValue*>*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSObject*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSFunction*) { return LSNull::null_var; }
LSValue* LSValue::operator += (const LSClass*) { return LSNull::null_var; }

//LSValue* LSValue::operator - (const LSValue*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSNull*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSBoolean*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSNumber*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSString*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSArray<LSValue*>*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSObject*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSFunction*) const { return LSNull::null_var; }
LSValue* LSValue::operator - (const LSClass*) const { return LSNull::null_var; }

//LSValue* LSValue::operator -= (LSValue*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSNull*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSBoolean*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSNumber*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSString*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSArray<LSValue*>*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSObject*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSFunction*) { return LSNull::null_var; }
LSValue* LSValue::operator -= (const LSClass*) { return LSNull::null_var; }

//LSValue* LSValue::operator * (const LSValue*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSNull*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSBoolean*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSNumber*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSString*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSArray<LSValue*>*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSObject*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSFunction*) const { return LSNull::null_var; }
LSValue* LSValue::operator * (const LSClass*) const { return LSNull::null_var; }

//LSValue* LSValue::operator *= (LSValue*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSNull*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSBoolean*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSNumber*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSString*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSArray<LSValue*>*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSObject*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSFunction*) { return LSNull::null_var; }
LSValue* LSValue::operator *= (const LSClass*) { return LSNull::null_var; }

//LSValue* LSValue::operator / (const LSValue*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSNull*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSBoolean*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSNumber*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSString*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSArray<LSValue*>*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSObject*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSFunction*) const { return LSNull::null_var; }
LSValue* LSValue::operator / (const LSClass*) const { return LSNull::null_var; }

//LSValue* LSValue::operator /= (LSValue*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSNull*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSBoolean*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSNumber*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSString*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSArray<LSValue*>*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSObject*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSFunction*) { return LSNull::null_var; }
LSValue* LSValue::operator /= (const LSClass*) { return LSNull::null_var; }

//LSValue* LSValue::poww(const LSValue*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSNull*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSBoolean*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSNumber*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSString*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSArray<LSValue*>*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSObject*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSFunction*) const { return LSNull::null_var; }
LSValue* LSValue::poww(const LSClass*) const { return LSNull::null_var; }

//LSValue* LSValue::pow_eq(LSValue*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSNull*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSBoolean*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSNumber*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSString*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSArray<LSValue*>*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSObject*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSFunction*) { return LSNull::null_var; }
LSValue* LSValue::pow_eq(const LSClass*) { return LSNull::null_var; }

//LSValue* LSValue::operator % (const LSValue*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSNull*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSBoolean*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSNumber*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSString*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSArray<LSValue*>*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSObject*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSFunction*) const { return LSNull::null_var; }
LSValue* LSValue::operator % (const LSClass*) const { return LSNull::null_var; }

//LSValue* LSValue::operator %= (LSValue*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSNull*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSBoolean*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSNumber*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSString*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSArray<LSValue*>*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSObject*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSFunction*) { return LSNull::null_var; }
LSValue* LSValue::operator %= (const LSClass*) { return LSNull::null_var; }

bool LSValue::operator == (const LSNull*) const { return false; }
bool LSValue::operator == (const LSBoolean*) const { return false; }
bool LSValue::operator == (const LSNumber*) const { return false; }
bool LSValue::operator == (const LSString*) const { return false; }
bool LSValue::operator == (const LSArray<LSValue*>*) const { return false; }
bool LSValue::operator == (const LSFunction*) const { return false; }
bool LSValue::operator == (const LSObject*) const { return false; }
bool LSValue::operator == (const LSClass*) const { return false; }

bool LSValue::in(const LSValue*) const { return false; }
LSValue* LSValue::abso() const { return LSNull::null_var; }

}

