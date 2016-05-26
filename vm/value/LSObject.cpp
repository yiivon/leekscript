#include "LSObject.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"

using namespace std;

LSValue* LSObject::object_class(new LSClass("Object"));

LSObject::LSObject() {
	clazz = nullptr;
	readonly = false;
}

LSObject::LSObject(initializer_list<pair<string, LSValue*>> values) {

	for (auto i : values) {
		this->values.insert(pair<string, LSValue*>(i.first, i.second->clone()));
	}
	clazz = nullptr;
}

LSObject::LSObject(LSClass* clazz) {
	this->clazz = clazz;
	readonly = false;
}

LSObject::LSObject(JsonValue& json) {
	clazz = nullptr;

	for (auto e : json) {
		addField(e->key, LSValue::parse(e->value));
	}
}

LSObject::~LSObject() {}

void LSObject::addField(string name, LSValue* var) {
	this->values.insert(pair<string, LSValue*>(name, var));
}

bool LSObject::isTrue() const {
	return values.size() > 0;
}

LSValue* LSObject::operator - () const {
	return this->clone();
}

LSValue* LSObject::operator ! () const {
	return this->clone();
}

LSValue* LSObject::operator ~ () const {
	return this->clone();
}

LSValue* LSObject::operator ++ () {
	return this;
}
LSValue* LSObject::operator ++ (int) {
	return LSNull::null_var;
}

LSValue* LSObject::operator -- () {
	return this;
}
LSValue* LSObject::operator -- (int) {
	return LSNull::null_var;
}

LSValue* LSObject::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSObject::operator + (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSArray<int>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator + (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSObject::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSObject::operator += (const LSNull*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSNumber*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSString*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSObject*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSFunction*) {
	return this->clone();
}
LSValue* LSObject::operator += (const LSClass*) {
	return this->clone();
}

LSValue* LSObject::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSObject::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSObject::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSObject::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator -= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator -= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSObject::operator -= (const LSString*) {
	return this->clone();
}
LSValue* LSObject::operator -= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSObject::operator -= (const LSObject*) {
	return this->clone();
}
LSValue* LSObject::operator -= (const LSFunction*) {
	return this->clone();
}
LSValue* LSObject::operator -= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSObject::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSObject::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator * (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSObject::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSObject::operator *= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator *= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator *= (const LSString*) {
	return this->clone();
}
LSValue* LSObject::operator *= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSObject::operator *= (const LSObject*) {
	return this->clone();
}
LSValue* LSObject::operator *= (const LSFunction*) {
	return this->clone();
}
LSValue* LSObject::operator *= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSObject::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSObject::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSObject::operator /= (LSValue* value) {
	return value->operator / (this);
}
LSValue* LSObject::operator /= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator /= (const LSString*) {
	return this->clone();
}
LSValue* LSObject::operator /= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSObject::operator /= (const LSObject*) {
	return this->clone();
}
LSValue* LSObject::operator /= (const LSFunction*) {
	return this->clone();
}
LSValue* LSObject::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSObject::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSObject::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSObject::poww(const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSObject::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
LSValue* LSObject::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSObject::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSObject::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSObject::pow_eq(const LSString*) {
	return this->clone();
}
LSValue* LSObject::pow_eq(const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSObject::pow_eq(const LSObject*) {
	return this->clone();
}
LSValue* LSObject::pow_eq(const LSFunction*) {
	return this->clone();
}
LSValue* LSObject::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSObject::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSObject::operator % (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSObject::operator % (const LSClass*) const {
	return LSNull::null_var;
}


LSValue* LSObject::operator %= (LSValue* value) {
	return value->operator %= (this);
}
LSValue* LSObject::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator %= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSObject::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSObject::operator %= (const LSString*) {
	return this->clone();
}
LSValue* LSObject::operator %= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSObject::operator %= (const LSObject*) {
	return this->clone();
}
LSValue* LSObject::operator %= (const LSFunction*) {
	return this->clone();
}
LSValue* LSObject::operator %= (const LSClass*) {
	return this->clone();
}

bool LSObject::operator == (const LSValue* value) const {
	return value->operator == (this);
}
bool LSObject::operator == (const LSNull*) const {
	return false;
}
bool LSObject::operator == (const LSBoolean*) const {
	return false;
}
bool LSObject::operator == (const LSNumber*) const {
	return false;
}
bool LSObject::operator == (const LSString*) const {
	return false;
}
bool LSObject::operator == (const LSArray<LSValue*>*) const {
	return false;
}
bool LSObject::operator == (const LSFunction*) const {
	return false;
}
bool LSObject::operator == (const LSObject*) const {
	return false;
}
bool LSObject::operator == (const LSClass*) const {
	return false;
}

bool LSObject::operator < (const LSValue* value) const {
	return value->operator < (this);
}
bool LSObject::operator < (const LSNull*) const {
	return false;
}
bool LSObject::operator < (const LSBoolean*) const {
	return false;
}
bool LSObject::operator < (const LSNumber*) const {
	return false;
}
bool LSObject::operator < (const LSString*) const {
	return false;
}
bool LSObject::operator < (const LSArray<LSValue*>*) const {
	return false;
}
bool LSObject::operator < (const LSObject* v) const {
	return values.size() < v->values.size();
}
bool LSObject::operator < (const LSFunction*) const {
	return true;
}
bool LSObject::operator < (const LSClass*) const {
	return true;
}

bool LSObject::operator > (const LSValue* value) const {
	return value->operator > (this);
}
bool LSObject::operator > (const LSNull*) const {
	return true;
}
bool LSObject::operator > (const LSBoolean*) const {
	return true;
}
bool LSObject::operator > (const LSNumber*) const {
	return true;
}
bool LSObject::operator > (const LSString*) const {
	return true;
}
bool LSObject::operator > (const LSArray<LSValue*>*) const {
	return true;
}
bool LSObject::operator > (const LSObject* v) const {
	return values.size() > v->values.size();
}
bool LSObject::operator > (const LSFunction*) const {
	return false;
}
bool LSObject::operator > (const LSClass*) const {
	return false;
}

bool LSObject::operator <= (const LSValue* value) const {
	return value->operator <= (this);
}
bool LSObject::operator <= (const LSNull*) const {
	return false;
}
bool LSObject::operator <= (const LSBoolean*) const {
	return false;
}
bool LSObject::operator <= (const LSNumber*) const {
	return false;
}
bool LSObject::operator <= (const LSString*) const {
	return false;
}
bool LSObject::operator <= (const LSArray<LSValue*>*) const {
	return false;
}
bool LSObject::operator <= (const LSObject* v) const {
	return values.size() <= v->values.size();
}
bool LSObject::operator <= (const LSFunction*) const {
	return true;
}
bool LSObject::operator <= (const LSClass*) const {
	return true;
}

bool LSObject::operator >= (const LSValue* value) const {
	return value->operator >= (this);
}
bool LSObject::operator >= (const LSNull*) const {
	return true;
}
bool LSObject::operator >= (const LSBoolean*) const {
	return true;
}
bool LSObject::operator >= (const LSNumber*) const {
	return true;
}
bool LSObject::operator >= (const LSString*) const {
	return true;
}
bool LSObject::operator >= (const LSArray<LSValue*>*) const {
	return true;
}
bool LSObject::operator >= (const LSObject* v) const {
	return values.size() >= v->values.size();
}
bool LSObject::operator >= (const LSFunction*) const {
	return false;
}
bool LSObject::operator >= (const LSClass*) const {
	return false;
}

LSValue* LSObject::at (const LSValue*) const {
	return LSNull::null_var;
}
LSValue** LSObject::atL (const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSObject::range(int, int) const {
	return this->clone();
}
LSValue* LSObject::rangeL(int, int) {
	return this;
}

bool LSObject::in(const LSValue* v) const {
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i->second->operator == (v)) {
			return true;
		}
	}
	return false;
}

LSValue* LSObject::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	try {
		return values.at(*((LSString*) key));
	} catch (exception& e) {
		if (clazz != nullptr) {
			string name = *((LSString*) key);
			LSFunction* attr = clazz->getDefaultMethod(name);
			if (attr != nullptr) {
				return (LSValue*) attr;
			}
		}
		return LSNull::null_var;
	}
}
LSValue** LSObject::attrL(const LSValue* key) {
	if (readonly) {
		return &LSNull::null_var;
	}
	try {
		return &values.at(*((LSString*) key));
	} catch (exception& e) {
		values.insert(pair<string, LSValue*>(*((LSString*) key), LSNull::null_var));
		return &values[*((LSString*) key)];
	}
}

LSValue* LSObject::abso() const {
	return LSNumber::get(values.size());
}

LSValue* LSObject::clone() const {
	LSObject* obj = new LSObject();
	for (auto i = values.begin(); i != values.end(); i++) {
		obj->values.insert(pair<string, LSValue*>(i->first, i->second->clone()));
	}
	return obj;
}

std::ostream& LSObject::print(std::ostream& os) const {
	if (clazz != nullptr) os << clazz->name << " ";
	os << "{";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) os << ", ";
		os << i->first;
		os << ": ";
		i->second->print(os);
	}
	os << "}";
	return os;
}

string LSObject::json() const {
	string res = "{";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) res += ",";
		res += "\"" + i->first + "\":";
		string json = i->second->to_json();
		res += json;
	}
	return res + "}";
}

LSValue* LSObject::getClass() const {
	if (clazz != nullptr) return clazz;
	return LSObject::object_class;
}

int LSObject::typeID() const {
	return 6;
}

const BaseRawType* LSObject::getRawType() const {
	return RawType::OBJECT;
}
