#include "LSObject.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

namespace ls {

LSValue* LSObject::object_class(new LSClass("Object"));

LSObject::LSObject() {
	clazz = nullptr;
	readonly = false;
}

LSObject::LSObject(initializer_list<pair<std::string, LSValue*>> values) {

	for (auto i : values) {
		addField(i.first, i.second->clone());
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
		LSValue::delete_ref(v.second);
	}
}

void LSObject::addField(string name, LSValue* var) {
	this->values.insert({name, var});
	var->refs++;
}

LSArray<LSValue*>* LSObject::ls_get_keys() const {
	LSArray<LSValue*>* keys = new LSArray<LSValue*>();
	for (auto i = values.begin(); i != values.end(); i++) {
		keys->push_no_clone(new LSString(i->first));
	}
	if (refs == 0) delete this;
	return keys;
}

LSArray<LSValue*>* LSObject::ls_get_values() const {
	LSArray<LSValue*>* v = new LSArray<LSValue*>();
	for (auto i = values.begin(); i != values.end(); i++) {
		v->push_clone(i->second);
	}
	if (refs == 0) delete this;
	return v;
}

/*
 * LSValue methods
 */

bool LSObject::isTrue() const {
	return values.size() > 0;
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
bool LSObject::operator == (const LSObject*) const {
	return false;
}

bool LSObject::operator < (const LSValue* value) const {
	return value->operator < (this);
}
bool LSObject::operator < (const LSObject* v) const {
	return values.size() < v->values.size();
}



LSValue* LSObject::at (const LSValue*) const {
	return LSNull::get();
}
LSValue** LSObject::atL (const LSValue*) {
	return nullptr;
}

bool LSObject::in(LSValue* v) const {
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
//		cout << "attr : " << values.at(*((LSString*) key))->refs << endl;
		return values.at(*((LSString*) key));
	} catch (exception& e) {
		if (clazz != nullptr) {
			string name = *((LSString*) key);
			LSFunction* attr = clazz->getDefaultMethod(name);
			if (attr != nullptr) {
				return (LSValue*) attr;
			}
		}
		return LSNull::get();
	}
}
LSValue** LSObject::attrL(const LSValue* key) {
	if (readonly) {
		return nullptr;
	}
	try {
		return &values.at(*((LSString*) key));
	} catch (exception& e) {
		return nullptr;
//		values.insert({*((LSString*) key), LSNull::get()});
//		return &values[*((LSString*) key)];
	}
}

LSValue* LSObject::abso() const {
	return LSNumber::get(values.size());
}

LSValue* LSObject::clone() const {
	LSObject* obj = new LSObject();
	for (auto i = values.begin(); i != values.end(); i++) {
		obj->values.insert({i->first, i->second->clone_inc()});
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
	return 9;
}

const BaseRawType* LSObject::getRawType() const {
	return RawType::OBJECT;
}

}
