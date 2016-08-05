#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include <exception>

#include "LSMap.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"

using namespace std;

template <class T>
LSMap<T>::LSMap() {
	index = 0;
}

template <class T>
LSMap<T>::LSMap(initializer_list<pair<LSValue*, T>> values) {
	cout << "init LSMap" << endl;
	index = 0;
	for (auto i : values) {
		if (i.first->isInteger()) {
			index = max(index, (int) ((LSNumber*)i.first)->value + 1);
		}
		this->values[i.first] = i.second;
	}
}

template <class T>
LSMap<T>::~LSMap() {}

template <class T>
LSValue* LSMap<T>::pop() {
	auto last = this->values.rbegin();
	if (last == this->values.rend()) {
		return LSNull::get();
	}
	index--;
	LSValue* val = (LSValue*) last->second;
	this->values.erase(last->first);
	return val;
}

template <class T>
void LSMap<T>::push_clone(const T value) {
	this->push_no_clone(value->clone());
}

template <class T>
void LSMap<T>::push_no_clone(T value) {
	LSValue* key = LSNumber::get(index++);
	this->values.insert(pair<LSValue*, T> (key, value));
}

template <class T>
void LSMap<T>::push_key_no_clone(LSValue* key, T var) {
	this->values[key] = var;
	if (key->isInteger()) {
		index = max(index, (int) ((LSNumber*)key)->value + 1);
	}
}

template <class T>
void LSMap<T>::push_key_clone(LSValue* key, const T var) {
	push_key_no_clone(key, (T) var->clone());
}

template <class T>
LSValue* LSMap<T>::remove_key(LSValue* key) {
	auto it = this->values.find(key);
	if (it != this->values.end()) {
		T val = it->second;
		this->values.erase(it);
		return val;
	}
	return LSNull::get();
}

template <class T>
void LSMap<T>::clear() {
	index = 0;
	values.clear();
}
template <class T>
size_t LSMap<T>::size() const {
	return values.size();
}

template <class T>
T LSMap<T>::sum() const {
//	double sum = 0;
//	for (auto v : this->values) {
//		sum += ((LSNumber*) v.second)->value;
//	}
//	return sum;
	return (T) LSNull::get();
}

template <class T>
double LSMap<T>::average() const {
	if (size() == 0) return 0;
//	return this->sum() / size();
	return 0;
}


/*
 * LSValue common methods
 */
template <class T>
bool LSMap<T>::isTrue() const {
	return values.size() > 0;
}

template <class T>
LSValue* LSMap<T>::operator ! () const {
	return LSBoolean::get(values.size() == 0);
}

template <class T>
LSValue* LSMap<T>::operator + (const LSNull* nulll) const {
	LSMap<LSValue*>* new_map = (LSMap<LSValue*>*) this->clone();
	new_map->push_clone((T) nulll);
	return new_map;
}

template <class T>
LSValue* LSMap<T>::operator + (const LSBoolean* boolean) const {
	LSMap<LSValue*>* new_map = (LSMap<LSValue*>*) this->clone();
	new_map->push_clone((T) boolean);
	return new_map;
}

template <class T>
LSValue* LSMap<T>::operator + (const LSNumber* number) const {
	LSMap<LSValue*>* new_map = (LSMap<LSValue*>*) this->clone();
	new_map->push_clone((T) number);
	return new_map;
}

template <class T>
LSValue* LSMap<T>::operator + (const LSString* string) const {
	LSMap<LSValue*>* new_map = (LSMap<LSValue*>*) this->clone();
	new_map->push_clone((T) string);
	return new_map;
}

template <class T>
LSValue* LSMap<T>::operator + (const LSArray<LSValue*>* array) const {

	LSMap<LSValue*>* new_map = new LSMap<LSValue*>();

	for (auto i = values.begin(); i != values.end(); ++i) {
		new_map->push_key_no_clone(i->first, i->second);
	}
	for (auto i = array->begin(); i != array->end(); i++) {
		new_map->push_clone(*i);
	}
	return new_map;
}

template <class T>
LSValue* LSMap<T>::operator + (const LSMap<LSValue*>* map) const {

	LSMap<LSValue*>* new_map = new LSMap<LSValue*>();

	for (auto i = values.begin(); i != values.end(); ++i) {
		new_map->push_key_clone(i->first, i->second);
	}
	for (auto i = map->values.begin(); i != map->values.end(); ++i) {
		new_map->push_key_clone(i->first, i->second);
	}
	return new_map;
}

template <class T>
LSValue* LSMap<T>::operator += (const LSMap<LSValue*>* map) {

	LSMap<LSValue*>* cmap = map;
	if ((void*)this == map) {
		cmap = map->clone();
	}
	for (auto i = cmap->values.begin(); i != cmap->values.end(); ++i) {
		push_key_clone(i->first, i->second);
	}
	if ((void*)this == map) {
		delete cmap;
	}
	return this;
}

template <class T>
bool LSMap<T>::operator == (const LSMap<LSValue*>* v) const {

	if (this->values.size() != v->values.size()) {
		return false;
	}
	auto i = values.begin();
	auto j = v->values.begin();

	for (; i != values.end(); i++, j++) {
		if (i->second->operator != (j->second)) return false;
	}
	return true;
}

template <class T>
bool LSMap<T>::in(const LSValue* key) const {

	for (auto i = values.begin(); i != values.end(); i++) {
		if (i->second->operator == (key)) {
			return true;
		}
	}
	return false;
}

template <class T>
LSValue* LSMap<T>::at(const LSValue* key) const {
	try {
		return (LSValue*) values.at((LSValue*) key);
	} catch (exception& e) {
		return LSNull::get();
	}
}

template <class T>
LSValue** LSMap<T>::atL(const LSValue* key) {
	try {
		return (LSValue**) &values[(LSValue*) key];
	} catch (exception& e) {
		return nullptr;
	}
}

template <class T>
LSValue* LSMap<T>::range(int start, int end) const {

	LSArray<LSValue*>* range = new LSArray<LSValue*>();

	LSNumber* start_p = LSNumber::get(start);
	LSNumber* end_p = LSNumber::get(end);

	for (auto i = values.begin(); i != values.end(); i++) {

		if (i->first->operator < (end_p)) break; // i > end
		if (start_p->operator < (i->first)) continue; // i < start

		range->push_clone(i->second);
	}
	return range;
}

template <class T>
LSValue* LSMap<T>::attr(const LSValue* key) const {

	if (key->operator == (new LSString("size"))) {
		return LSNumber::get(this->values.size());
	}
	if (*((LSString*) key) == "class") {
		return this->getClass();
	}

	try {
		return (LSValue*) values.at((LSValue*) key);
	} catch (exception& e) {
		return LSNull::get();
	}
}

template <class T>
LSValue* LSMap<T>::abso() const {
	return LSNumber::get(values.size());
}

template <class T>
LSValue* LSMap<T>::clone() const {

	LSMap<T>* new_map = new LSMap<T>();
	new_map->index = index;

	for (auto i = values.begin(); i != values.end(); i++) {
		new_map->push_key_clone(i->first, i->second);
	}
	return new_map;
}

template <class T>
std::ostream& LSMap<T>::print(std::ostream& os) const {

	os << "[";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) os << ", ";
		i->first->print(os);
		os << ": ";
		i->second->print(os);
	}
	os << "]";
	return os;
}

template <class T>
string LSMap<T>::json() const {
	string res = "[";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) res += ",";
		string json = i->second->to_json();
		res += json;
	}
	return res + "]";
}

#endif
