#include "LSObject.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

namespace ls {

LSValue* LSObject::object_class(new LSClass("Object", 1));

LSObject::LSObject() {
	clazz = nullptr;
	readonly = false;
}

LSObject::LSObject(initializer_list<pair<std::string, LSValue*>> values) {

	for (auto i : values) {
		this->values.insert({i.first, i.second->clone()});
	}
	clazz = nullptr;
	readonly = false;
}

LSObject::LSObject(LSClass* clazz) {
	this->clazz = clazz;
	readonly = false;
}

LSObject::LSObject(Json& json) {

	clazz = nullptr;

	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		addField(it.key(), LSValue::parse(it.value()));
	}
}

LSObject::~LSObject() {
	for (auto v : values) {
		LSValue::delete_val(v.second);
	}
}

void LSObject::addField(string name, LSValue* var) {
	this->values.insert({name, var});
	var->refs++;
}

LSArray<LSString*>* LSObject::get_keys() const {
	LSArray<LSString*>* keys = new LSArray<LSString*>();
	for (auto i = values.begin(); i != values.end(); i++) {
		keys->push_no_clone(new LSString(i->first));
	}
	return keys;
}

LSArray<LSValue*>* LSObject::get_values() const {
	LSArray<LSValue*>* v = new LSArray<LSValue*>();
	for (auto i = values.begin(); i != values.end(); i++) {
		v->push_clone(i->second);
	}
	return v;
}

/*
 * LSValue methods
 */

bool LSObject::isTrue() const {
	return values.size() > 0;
}

LSValue* LSObject::operator - () const {
	return LSNull::null_var;
}

LSValue* LSObject::operator ! () const {
	return LSNull::null_var;
}

LSValue* LSObject::operator ~ () const {
	return LSNull::null_var;
}

LSValue* LSObject::operator ++ () {
	return LSNull::null_var;
}
LSValue* LSObject::operator ++ (int) {
	return LSNull::null_var;
}

LSValue* LSObject::operator -- () {
	return LSNull::null_var;
}
LSValue* LSObject::operator -- (int) {
	return LSNull::null_var;
}

LSValue* LSObject::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSObject::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSObject::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSObject::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSObject::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSObject::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSObject::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSObject::operator /= (LSValue* value) {
	return value->operator / (this);
}
LSValue* LSObject::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSObject::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
LSValue* LSObject::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSObject::operator %= (LSValue* value) {
	return value->operator %= (this);
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
bool LSObject::operator < (const LSArray<int>* v) const {
	return false;
}
bool LSObject::operator < (const LSArray<double>* v) const {
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
		return values.at(*((LSString*) key))->clone();
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
		values.insert({*((LSString*) key), LSNull::null_var});
		return &values[*((LSString*) key)];
	}
}

LSValue* LSObject::abso() const {
	return LSNumber::get(values.size());
}

LSValue* LSObject::clone() const {
	LSObject* obj = new LSObject();
	for (auto i = values.begin(); i != values.end(); i++) {
		obj->values.insert({i->first, i->second->clone()});
	}
	return obj;
}

std::ostream& LSObject::print(std::ostream& os) const {
	if (clazz != nullptr) os << clazz->name << L" ";
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

std::string LSObject::json() const {
	std::string res = "{";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) res += ",";
		res += "\"" + i->first + "\":";
		std::string json = i->second->to_json();
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

}
