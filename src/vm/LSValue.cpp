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
using namespace ls;

int LSValue::obj_count = 0;
int LSValue::obj_deleted = 0;

LSValue::LSValue() {
//	cout << "LSValue()" << endl;
	native = false;
	obj_count++;
}

LSValue::~LSValue() {
//	cout << "~LSValue()" << endl;
	obj_deleted++;
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
