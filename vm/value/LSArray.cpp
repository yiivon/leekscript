#include "LSArray.hpp"
#include "../LSValue.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include "LSNumber.hpp"
#include "LSBoolean.hpp"
#include <algorithm>

using namespace std;

LSValue* LSArray::array_class(new LSClass("Array"));

LSArray::LSArray() {
	associative = false;
	index = 0;
}

LSArray::LSArray(initializer_list<LSValue*> values_list) {
	associative = false;
	index = 0;
	for (auto i : values_list) {
		LSValue** v = &this->values[new LSNumber(index++)];
		*v = i;
	}
}

LSArray::LSArray(initializer_list<pair<LSValue*, LSValue*>> values) {
	associative = true;
	index = 0;
	for (auto i : values) {
		if (i.first->isInteger()) {
			index = max(index, (int) ((LSNumber*)i.first)->value + 1);
		}
		LSValue** v = &this->values[i.first];
		*v = i.second;
	}
}

LSArray::LSArray(JsonValue& json) {
	index = 0;
	associative = false;

	for (auto e : json) {
		pushClone(LSValue::parse(e->value));
	}
}

LSArray::~LSArray() {}

void LSArray::clear() {
	associative = false;
	index = 0;
	values.clear();
}

LSValue* LSArray::remove(LSNumber* index) {
	// TODO : move all indices after index to the left ?
	// or transform the array into a associative one
	return removeKey(index);
}

LSValue* LSArray::removeKey(LSValue* key) {
	auto it = this->values.find(key);
	if (it != this->values.end()) {
		associative = true;
		LSValue* val = it->second;
		this->values.erase(it);
		return val;
	}
	return LSNull::null_var;
}

LSValue* LSArray::pop() {
	auto last = this->values.rbegin();
	if (last == this->values.rend())
		return LSNull::null_var;
	index--;
	LSValue* val = last->second;
	this->values.erase(last->first);
	return val;
}

void LSArray::pushNoClone(LSValue *value) {

	LSValue* key = LSNumber::get(index++);
	LSValue* val = value;

	this->values.insert(pair<LSValue*, LSValue*> (key, val));
}

void LSArray::pushKeyNoClone(LSValue *key, LSValue *var) {
	this->values[key] = var;
	associative = true;
	if (key->isInteger()) {
		index = max(index, (int) ((LSNumber*)key)->value + 1);
	}
}

void LSArray::pushClone(LSValue* value) {
	pushNoClone(value->clone());
}

void LSArray::pushKeyClone(LSValue* key, LSValue* var) {
	pushKeyNoClone(key, var->clone());
}

LSArrayIterator LSArray::begin() {
	return values.begin();
}

LSArrayIterator LSArray::end() {
	return values.end();
}

LSValue* LSArray::at(LSArrayIterator it) {
	return it->second;
}

bool LSArray::isTrue() const {
	return values.size() > 0;
}

LSValue* LSArray::operator - () const {
	return this->clone();
}

LSValue* LSArray::operator ! () const {
	return LSBoolean::get(values.size() == 0);
}

LSValue* LSArray::operator ~ () const {
	LSArray* array = new LSArray();
	for (auto i = values.rbegin(); i != values.rend(); ++i) {
		array->pushClone(i->second);
	}
	return array;
}

LSValue* LSArray::operator ++ () {
	return this;
}

LSValue* LSArray::operator ++ (int) {
	return this;
}

LSValue* LSArray::operator -- () {
	return this;
}

LSValue* LSArray::operator -- (int) {
	return LSNull::null_var;
}

LSValue* LSArray::operator + (const LSValue* v) const {
	return v->operator + (this);
}

LSValue* LSArray::operator + (const LSNull* nulll) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), (LSValue*)nulll));
	return newArray;
}

LSValue* LSArray::operator + (const LSBoolean* boolean) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), boolean->clone()));
	return newArray;
}

LSValue* LSArray::operator + (const LSNumber* number) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), number->clone()));
	return newArray;
}

LSValue* LSArray::operator + (const LSString* string) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), string->clone()));
	return newArray;
}

LSValue* LSArray::operator + (const LSArray* array) const {

	LSArray* newArray = new LSArray();
	newArray->associative = array->associative or associative;
	newArray->index = 0;

	for (auto i = values.begin(); i != values.end(); ++i) {
		if (associative) {
			newArray->values.insert(pair<LSValue*, LSValue*>(i->first, i->second->clone()));
		} else {
			newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), i->second->clone()));
		}
	}
	for (auto i = array->values.begin(); i != array->values.end(); ++i) {
		if (array->associative) {
			newArray->values.insert(pair<LSValue*, LSValue*>(i->first, i->second->clone()));
		} else {
			newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), i->second->clone()));
		}
	}

	return newArray;
}

LSValue* LSArray::operator + (const LSObject* object) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), (LSValue*) object));
	return newArray;
}

LSValue* LSArray::operator + (const LSFunction* fun) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), (LSValue*) fun));
	return newArray;
}

LSValue* LSArray::operator + (const LSClass* clazz) const {
	LSArray* newArray = (LSArray*) clone();
	newArray->values.insert(pair<LSValue*, LSValue*>(LSNumber::get(newArray->index++), (LSValue*) clazz));
	return newArray;
}

LSValue* LSArray::operator += (LSValue* value) const {
	return value->operator += (this);
}

LSValue* LSArray::operator += (const LSNull* nulll) {
	pushClone((LSValue*) nulll);
	return this;
}

LSValue* LSArray::operator += (const LSBoolean* boolean) {
	pushClone((LSValue*) boolean);
	return this;
}

LSValue* LSArray::operator += (const LSNumber* number) {
	pushClone((LSValue*) number);
	return this;
}

LSValue* LSArray::operator += (const LSString* string) {
	pushClone((LSValue*) string);
	return this;
}

LSValue* LSArray::operator += (const LSArray* array) {

	LSArray* arr = (LSArray*) array;

	// Same array ?
	if (array == this) {
		arr = (LSArray*) array->clone();
	}

	for (auto i = arr->values.begin(); i != arr->values.end(); ++i) {
		if (arr->associative) {
			LSValue** v = &values[i->first];
			*v = i->second;
		} else {
			LSValue** v = &values[LSNumber::get(index++)];
			*v = i->second;
		}
	}

	if (array == this) {
		delete arr;
	}

	return this;
}

LSValue* LSArray::operator += (const LSObject* object) {
	pushClone((LSValue*) object);
	return this;
}

LSValue* LSArray::operator += (const LSFunction* function) {
	pushClone((LSValue*) function);
	return this;
}

LSValue* LSArray::operator += (const LSClass* clazz) {
	pushClone((LSValue*) clazz);
	return this;
}


LSValue* LSArray::operator - (const LSValue* value) const {
	return value->operator - (this);
}

LSValue* LSArray::operator - (const LSNull*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator - (const LSNumber* number) const {

	LSArray* copy = (LSArray*) clone();

	for (auto i = copy->values.begin(); i != copy->values.end(); ++i) {
		if (i->second->operator == (number)) {
			copy->values.erase(i);
		}
	}
	return copy;
}

LSValue* LSArray::operator - (const LSString*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator - (const LSArray*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator - (const LSObject*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator - (const LSFunction*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator - (const LSClass*) const {
	return LSNull::null_var;
}


LSValue* LSArray::operator -= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSArray::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator -= (const LSClass*) {
	return LSNull::null_var;
}


LSValue* LSArray::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSArray::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator * (const LSClass*) const {
	return LSNull::null_var;
}


LSValue* LSArray::operator *= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSArray::operator *= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator *= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSArray::operator / (const LSValue* value) const {
	return value->operator / (value);
}
LSValue* LSArray::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator /= (LSValue* value) const {
	return value->operator -= (this);
}
LSValue* LSArray::operator /= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSArray::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSArray::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSArray::poww(const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSArray::pow_eq(LSValue* value) const {
	return value->pow_eq(this);
}
LSValue* LSArray::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSString*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSArray::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSArray::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSArray::operator % (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSArray*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSArray::operator % (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSArray::operator %= (LSValue* value) const {
	return value->operator %= (this);
}
LSValue* LSArray::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSArray*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSArray::operator %= (const LSClass*) {
	return LSNull::null_var;
}

bool LSArray::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSArray::operator == (const LSNull* v) const {
	return v->operator == (this);
}
bool LSArray::operator == (const LSBoolean*) const {
	return false;
}
bool LSArray::operator == (const LSNumber*) const {
	return false;
}
bool LSArray::operator == (const LSString*) const {
	return false;
}
bool LSArray::operator == (const LSArray* v) const {

	if (this->values.size() != v->values.size()) {
		return false;
	}
	for (auto i = values.begin(), j = v->values.begin(); i != values.end(); i++, j++) {
		if (i->second->operator != (j->second)) return false;
	}
	return true;
}
bool LSArray::operator == (const LSObject*) const {
	return false;
}
bool LSArray::operator == (const LSFunction*) const {
	return false;
}
bool LSArray::operator == (const LSClass*) const {
	return false;
}

bool LSArray::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSArray::operator < (const LSNull*) const {
	return false;
}
bool LSArray::operator < (const LSBoolean*) const {
	return false;
}
bool LSArray::operator < (const LSNumber*) const {
	return false;
}
bool LSArray::operator < (const LSString*) const {
	return false;
}
bool LSArray::operator < (const LSArray* v) const {
	return this->values.size() < v->values.size();
}
bool LSArray::operator < (const LSObject*) const {
	return true;
}
bool LSArray::operator < (const LSFunction*) const {
	return true;
}
bool LSArray::operator < (const LSClass*) const {
	return true;
}

bool LSArray::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSArray::operator > (const LSNull*) const {
	return true;
}
bool LSArray::operator > (const LSBoolean*) const {
	return true;
}
bool LSArray::operator > (const LSNumber*) const {
	return true;
}
bool LSArray::operator > (const LSString*) const {
	return true;
}
bool LSArray::operator > (const LSArray* v) const {
	return this->values.size() > v->values.size();
}
bool LSArray::operator > (const LSObject*) const {
	return false;
}
bool LSArray::operator > (const LSFunction*) const {
	return false;
}
bool LSArray::operator > (const LSClass*) const {
	return false;
}

bool LSArray::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSArray::operator <= (const LSNull*) const {
	return false;
}
bool LSArray::operator <= (const LSBoolean*) const {
	return false;
}
bool LSArray::operator <= (const LSNumber*) const {
	return false;
}
bool LSArray::operator <= (const LSString*) const {
	return false;
}
bool LSArray::operator <= (const LSArray* v) const {
	return this->values.size() <= v->values.size();
}
bool LSArray::operator <= (const LSObject*) const {
	return true;
}
bool LSArray::operator <= (const LSFunction*) const {
	return true;
}
bool LSArray::operator <= (const LSClass*) const {
	return true;
}

bool LSArray::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSArray::operator >= (const LSNull*) const {
	return true;
}
bool LSArray::operator >= (const LSBoolean*) const {
	return true;
}
bool LSArray::operator >= (const LSNumber*) const {
	return true;
}
bool LSArray::operator >= (const LSString*) const {
	return true;
}
bool LSArray::operator >= (const LSArray* v) const {
	return this->values.size() >= v->values.size();
}
bool LSArray::operator >= (const LSObject*) const {
	return false;
}
bool LSArray::operator >= (const LSFunction*) const {
	return false;
}
bool LSArray::operator >= (const LSClass*) const {
	return false;
}

bool LSArray::in(const LSValue* key) const {
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i->second->operator == (key)) {
			return true;
		}
	}
	return false;
}

LSValue* LSArray::at(const LSValue* key) const {
	try {
		return (LSValue*) values.at((LSValue*) key);
	} catch (exception& e) {
		return LSNull::null_var;
	}
}

LSValue** LSArray::atL(const LSValue* key) {
	try {
		return (LSValue**) &values.at((LSValue*) key);
	} catch (exception& e) {
		return &LSNull::null_var;
	}
}

/*
 * Quick implementation
 */
LSValue* LSArray::range(const LSValue* start, const LSValue* end) const {

	LSArray* range = new LSArray();

	for (auto i = values.begin(); i != values.end(); i++) {

		if (i->first->operator < (end)) break; // i > end
		if (start->operator < (i->first)) continue; // i < start

		range->pushClone(i->second);
	}
	return range;
}

LSValue* LSArray::rangeL(const LSValue*, const LSValue*) {
	return this;
}

LSValue* LSArray::attr(const LSValue* key) const {

	if (key->operator == (new LSString("size"))) {
		return LSNumber::get(this->values.size());
	}
	if (((LSString*) key)->value == "class") {
		return getClass();
	}

	try {
		return values.at((LSValue*) key);
	} catch (exception& e) {
		return LSNull::null_var;
	}
}

LSValue** LSArray::attrL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSArray::abso() const {
	return LSNumber::get(values.size());
}

LSValue* LSArray::clone() const {

	LSArray* newArray = new LSArray();
	newArray->associative = associative;
	newArray->index = index;

	for (auto i = values.begin(); i != values.end(); i++) {
		LSValue** v = &newArray->values[i->first];
		*v = i->second->clone();
	}
	return newArray;
}

std::ostream& LSArray::print(std::ostream& os) const {
	os << "[";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) os << ", ";
		if (associative) {
			i->first->print(os);
			os << ": ";
		}
		i->second->print(os);
	}
	os << "]";
	return os;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

string LSArray::json() const {
	string res = "[";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) res += ",";
		string json = i->second->to_json();
		res += json;
	}
	return res + "]";
}

LSValue* LSArray::getClass() const {
	return LSArray::array_class;
}

int LSArray::typeID() const {
	return 5;
}

RawType LSArray::getRawType() const {
	return RawType::ARRAY;
}
