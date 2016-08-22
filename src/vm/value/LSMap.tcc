#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include "LSNumber.hpp"
#include "LSNull.hpp"

#include <exception>

namespace ls {

template <>
inline bool lsmap_less<LSValue*>::operator()(LSValue* lhs, LSValue* rhs) const {
	return rhs->operator < (lhs);
}

template <typename K>
inline bool lsmap_less<K>::operator()(K lhs, K rhs) const {
	return lhs < rhs;
}

template <typename K, typename T>
LSValue* LSMap<K,T>::map_class(new LSClass("Map"));

template <typename K, typename T>
inline LSMap<K, T>::LSMap() {}

template <>
inline LSMap<LSValue*,LSValue*>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
		LSValue::delete_ref(it->second);
	}
}
template <>
inline LSMap<LSValue*,int>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
}
template <>
inline LSMap<LSValue*,double>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
}
template <>
inline LSMap<int,LSValue*>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->second);
	}
}
template <>
inline LSMap<int,int>::~LSMap() {}
template <>
inline LSMap<int,double>::~LSMap() {}

/*
 * Map methods
 */
template <>
inline bool LSMap<LSValue*,LSValue*>::ls_insert(LSValue* key, LSValue* value) {
	auto it = lower_bound(key);
	if (it == end() || it->first->operator !=(key)) {
		emplace_hint(it, key->move_inc(), value->move_inc());
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(key);
	LSValue::delete_temporary(value);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,int>::ls_insert(LSValue* key, int value) {
	auto it = lower_bound(key);
	if (it == end() || it->first->operator !=(key)) {
		emplace_hint(it, key->move_inc(), value);
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(key);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,double>::ls_insert(LSValue* key, double value) {
	auto it = lower_bound(key);
	if (it == end() || it->first->operator !=(key)) {
		emplace_hint(it, key->move_inc(), value);
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(key);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,LSValue*>::ls_insert(int key, LSValue* value) {
	auto it = lower_bound(key);
	if (it == end() || it->first != key) {
		emplace_hint(it, key, value->move_inc());
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(value);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,int>::ls_insert(int key, int value) {
	bool r = emplace(key, value).second;
	if (refs == 0) delete this;
	return r;
}
template <>
inline bool LSMap<int,double>::ls_insert(int key, double value) {
	bool r = emplace(key, value).second;
	if (refs == 0) delete this;
	return r;
}


template <>
inline LSMap<LSValue*,LSValue*>* LSMap<LSValue*,LSValue*>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
		LSValue::delete_ref(it->second);
	}
	clear();
	return this;
}
template <>
inline LSMap<LSValue*,int>* LSMap<LSValue*,int>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
	clear();
	return this;
}
template <>
inline LSMap<LSValue*,double>* LSMap<LSValue*,double>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
	clear();
	return this;
}
template <>
inline LSMap<int,LSValue*>* LSMap<int,LSValue*>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->second);
	}
	clear();
	return this;
}
template <>
inline LSMap<int,int>* LSMap<int,int>::ls_clear() {
	clear();
	return this;
}
template <>
inline LSMap<int,double>* LSMap<int,double>::ls_clear() {
	clear();
	return this;
}

template <>
inline bool LSMap<LSValue*,LSValue*>::ls_erase(LSValue* key) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_ref(it->first);
		LSValue::delete_ref(it->second);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,int>::ls_erase(LSValue* key) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_ref(it->first);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,double>::ls_erase(LSValue* key) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_ref(it->first);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,LSValue*>::ls_erase(int key) {
	auto it = find(key);
	if (it != end()) {
		LSValue::delete_ref(it->second);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,int>::ls_erase(int key) {
	bool r = erase(key);
	if (refs == 0) delete this;
	return r;
}
template <>
inline bool LSMap<int,double>::ls_erase(int key) {
	bool r = erase(key);
	if (refs == 0) delete this;
	return r;
}


template <>
inline LSValue* LSMap<LSValue*,LSValue*>::ls_look(LSValue* key, LSValue* def) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_temporary(def);
		if (refs == 0) {
			LSValue* r = it->second->clone();
			delete this;
			return r;
		}
		return it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline int LSMap<LSValue*,int>::ls_look(LSValue* key, int def) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline double LSMap<LSValue*,double>::ls_look(LSValue* key, double def) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline LSValue* LSMap<int,LSValue*>::ls_look(int key, LSValue* def) {
	auto it = find(key);
	if (it != end()) {
		LSValue::delete_temporary(def);
		if (refs == 0) {
			LSValue* r = it->second->clone();
			delete this;
			return r;
		}
		return it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline int LSMap<int,int>::ls_look(int key, int def) {
	auto it = find(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline double LSMap<int,double>::ls_look(int key, double def) {
	auto it = find(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}


/*
 * LSValue methods
 */

template <typename K, typename T>
inline bool LSMap<K,T>::isTrue() const {
	return not this->empty();
}



template <typename K, typename T>
inline bool LSMap<K,T>::operator == (const LSValue* value) const {
	return value->operator == (this);
}

template <>
inline bool LSMap<LSValue*,LSValue*>::operator == (const LSMap<LSValue*,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->first->operator !=(it2->first)) return false;
		if (it1->second->operator !=(it2->second)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,int>::operator == (const LSMap<LSValue*,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* v2 = dynamic_cast<LSNumber*>(it2->second);
		if (!v2) return false;
		if (it1->second != v2->value) return false;
		if (it1->first->operator !=(it2->first)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,double>::operator == (const LSMap<LSValue*,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* v2 = dynamic_cast<LSNumber*>(it2->second);
		if (!v2) return false;
		if (it1->second != v2->value) return false;
		if (it1->first->operator !=(it2->first)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,LSValue*>::operator == (const LSMap<LSValue*,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		if (it1->first != k2->value) return false;
		if (it1->second->operator !=(it2->second)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,int>::operator == (const LSMap<LSValue*,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		LSNumber* v2 = dynamic_cast<LSNumber*>(it2->second);
		if (!v2) return false;
		if (it1->first != k2->value) return false;
		if (it1->second != v2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,double>::operator == (const LSMap<LSValue*,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		LSNumber* v2 = dynamic_cast<LSNumber*>(it2->second);
		if (!v2) return false;
		if (it1->first != k2->value) return false;
		if (it1->second != v2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,LSValue*>::operator == (const LSMap<LSValue*,int>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,int>::operator == (const LSMap<LSValue*,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->second != it2->second) return false;
		if (it1->first->operator !=(it2->first)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,double>::operator == (const LSMap<LSValue*,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->second != it2->second) return false;
		if (it1->first->operator !=(it2->first)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,LSValue*>::operator == (const LSMap<LSValue*,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		LSNumber* v1 = dynamic_cast<LSNumber*>(it1->second);
		if (!v1) return false;
		if (it1->first != k2->value) return false;
		if (v1->value != it2->second) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,int>::operator == (const LSMap<LSValue*,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		if (it1->second != it2->second) return false;
		if (it1->first != k2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,double>::operator == (const LSMap<LSValue*,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		if (it1->second != it2->second) return false;
		if (it1->first != k2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,LSValue*>::operator == (const LSMap<LSValue*,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,int>::operator == (const LSMap<LSValue*,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,double>::operator == (const LSMap<LSValue*,double>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->second != it2->second) return false;
		if (it1->first->operator !=(it2->first)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,LSValue*>::operator == (const LSMap<LSValue*,double>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		LSNumber* v1 = dynamic_cast<LSNumber*>(it1->second);
		if (!v1) return false;
		if (it1->first != k2->value) return false;
		if (v1->value != it2->second) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,int>::operator == (const LSMap<LSValue*,double>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		if (it1->second != it2->second) return false;
		if (it1->first != k2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,double>::operator == (const LSMap<LSValue*,double>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* k2 = dynamic_cast<LSNumber*>(it2->first);
		if (!k2) return false;
		if (it1->second != it2->second) return false;
		if (it1->first != k2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,LSValue*>::operator == (const LSMap<int,LSValue*>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,int>::operator == (const LSMap<int,LSValue*>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,double>::operator == (const LSMap<int,LSValue*>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<int,LSValue*>::operator == (const LSMap<int,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->first != it2->first) return false;
		if (it1->second->operator !=(it2->second)) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,int>::operator == (const LSMap<int,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* v2 = dynamic_cast<LSNumber*>(it2->second);
		if (!v2) return false;
		if (it1->first != it2->first) return false;
		if (it1->second != v2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,double>::operator == (const LSMap<int,LSValue*>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		LSNumber* v2 = dynamic_cast<LSNumber*>(it2->second);
		if (!v2) return false;
		if (it1->first != it2->first) return false;
		if (it1->second != v2->value) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<LSValue*,LSValue*>::operator == (const LSMap<int,int>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,int>::operator == (const LSMap<int,int>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,double>::operator == (const LSMap<int,int>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<int,LSValue*>::operator == (const LSMap<int,int>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<int,int>::operator == (const LSMap<int,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->first != it2->first) return false;
		if (it1->second != it2->second) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}
template <>
inline bool LSMap<int,double>::operator == (const LSMap<int,int>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->first != it2->first) return false;
		if (it1->second != it2->second) return false;
		++it2;
		++it1;
	}
	return it2 == value->end();
}

template <>
inline bool LSMap<LSValue*,LSValue*>::operator == (const LSMap<int,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,int>::operator == (const LSMap<int,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<LSValue*,double>::operator == (const LSMap<int,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<int,LSValue*>::operator == (const LSMap<int,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<int,int>::operator == (const LSMap<int,double>* value) const {
	return value->operator ==(this);
}
template <>
inline bool LSMap<int,double>::operator == (const LSMap<int,double>* value) const {
	if (this->size() != value->size()) return false;
	auto it1 = begin();
	auto it2 = value->begin();
	while (it1 != end()) {
		if (it2 == value->end()) return false;
		if (it1->first != it2->first) return false;
		if (it1->second != it2->second) return false;
		++it2;
		++it1;
	}
	return true;
}


template <typename K, typename T>
inline bool LSMap<K,T>::operator <(const LSValue* value) const {
	return value->operator <(this);
}

template <>
inline LSValue* LSMap<LSValue*,LSValue*>::at(const LSValue* key) const {

	try {
		return (LSValue*) ((std::map<LSValue*,LSValue*>*) this)->at((LSValue*) key)->clone();
	} catch (std::exception&) {
		return LSNull::get();
	}
}
template <>
inline LSValue* LSMap<LSValue*,int>::at(const LSValue* key) const {

	try {
		return LSNumber::get(((std::map<LSValue*,int>*) this)->at((LSValue*) key));
	} catch (std::exception&) {
		return LSNull::get();
	}
}
template <>
inline LSValue* LSMap<LSValue*,double>::at(const LSValue* key) const {

	try {
		return LSNumber::get(((std::map<LSValue*,double>*) this)->at((LSValue*) key));
	} catch (std::exception&) {
		return LSNull::get();
	}
}
template <>
inline LSValue* LSMap<int,LSValue*>::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue*) ((std::map<int,LSValue*>*) this)->at((int) n->value)->clone();
		} catch (std::exception&) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}
template <>
inline LSValue* LSMap<int,int>::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::map<int,int>*) this)->at((int) n->value));
		} catch (std::exception&) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}
template <>
inline LSValue* LSMap<int,double>::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::map<int,double>*) this)->at((int) n->value));
		} catch (std::exception&) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}


template <typename K, typename T>
inline LSValue** LSMap<K,T>::atL(const LSValue*) {
	return &LSNull::null_var;
}
template <>
inline LSValue** LSMap<LSValue*,LSValue*>::atL(const LSValue* key) {
	try {
		return (LSValue**) &((std::map<LSValue*,LSValue*>*) this)->at((LSValue*) key);
	} catch (std::exception&) {
		return nullptr;
	}
}
template <>
inline LSValue** LSMap<int,LSValue*>::atL(const LSValue* key) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue**) &((std::map<int,LSValue*>*) this)->at((int) n->value);
		} catch (std::exception&) {
			return nullptr;
		}
	}
	return nullptr;
}


template <>
inline std::ostream& LSMap<LSValue*,LSValue*>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		it->first->print(os);
		os << " : ";
		it->second->print(os);
	}
	if (empty()) os << ':';
	return os << "]";
}
template <>
inline std::ostream& LSMap<LSValue*,int>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		it->first->print(os);
		os << " : " << it->second;
	}
	if (empty()) os << ':';
	return os << "]";
}
template <>
inline std::ostream& LSMap<LSValue*,double>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		it->first->print(os);
		os << " : " << it->second;
	}
	if (empty()) os << ':';
	return os << "]";
}
template <>
inline std::ostream& LSMap<int,LSValue*>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : ";
		it->second->print(os);
	}
	if (empty()) os << ':';
	return os << "]";
}
template <>
inline std::ostream& LSMap<int,int>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : " << it->second;
	}
	if (empty()) os << ':';
	return os << "]";
}
template <>
inline std::ostream& LSMap<int,double>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : " << it->second;
	}
	if (empty()) os << ':';
	return os << "]";
}

template <>
inline std::string LSMap<LSValue*,LSValue*>::json() const {
	std::string res = "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) res += ",";
		res += it->first->to_json();
		res += ":";
		res += it->second->to_json();
	}
	return res + "]";
}

template <>
inline std::string LSMap<LSValue*,int>::json() const {
	std::string res = "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) res += ",";
		res += it->first->to_json();
		res += ":";
		res += std::to_string(it->second);
	}
	return res + "]";
}
template <>
inline std::string LSMap<LSValue*,double>::json() const {
	std::string res = "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) res += ",";
		res += it->first->to_json();
		res += ":";
		res += std::to_string(it->second);
	}
	return res + "]";
}
template <>
inline std::string LSMap<int,LSValue*>::json() const {
	std::string res = "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) res += ",";
		res += std::to_string(it->first);
		res += ":";
		res += it->second->to_json();
	}
	return res + "]";
}
template <>
inline std::string LSMap<int,int>::json() const {
	std::string res = "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) res += ",";
		res += std::to_string(it->first);
		res += ":";
		res += std::to_string(it->second);
	}
	return res + "]";
}
template <>
inline std::string LSMap<int,double>::json() const {
	std::string res = "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) res += ",";
		res += std::to_string(it->first);
		res += ":";
		res += std::to_string(it->second);
	}
	return res + "]";
}

template <>
inline LSValue* LSMap<LSValue*,LSValue*>::clone() const {
	LSMap<LSValue*,LSValue*>* map = new LSMap<LSValue*,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone_inc(), it->second->clone_inc());
	}
	return map;
}
template <>
inline LSValue* LSMap<LSValue*,int>::clone() const {
	LSMap<LSValue*,int>* map = new LSMap<LSValue*,int>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone_inc(), it->second);
	}
	return map;
}
template <>
inline LSValue* LSMap<LSValue*,double>::clone() const {
	LSMap<LSValue*,double>* map = new LSMap<LSValue*,double>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone_inc(), it->second);
	}
	return map;
}
template <>
inline LSValue* LSMap<int,LSValue*>::clone() const {
	LSMap<int,LSValue*>* map = new LSMap<int,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first, it->second->clone_inc());
	}
	return map;
}
template <>
inline LSValue* LSMap<int,int>::clone() const {
	return new LSMap<int,int>(*this);
}
template <>
inline LSValue* LSMap<int,double>::clone() const {
	return new LSMap<int,double>(*this);
}

template <typename K, typename T>
inline LSValue* LSMap<K,T>::getClass() const {
	return LSMap<K,T>::map_class;
}

template <typename K, typename T>
inline int LSMap<K,T>::typeID() const {
	return 6;
}

template <typename K, typename T>
inline const BaseRawType* LSMap<K,T>::getRawType() const {
	return RawType::MAP;
}



}

#endif
