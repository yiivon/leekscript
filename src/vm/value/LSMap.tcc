#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include "LSNumber.hpp"
#include "LSNull.hpp"

#include <exception>

namespace ls {

//template<>
//class LSMap<LSValue*, LSValue*> {};

template <class K>
inline bool lsmap_less<K>::operator()(K lhs, K rhs) const {
	return rhs->operator < (lhs);
}

template <>
inline bool lsmap_less<int>::operator()(int lhs, int rhs) const {
	return lhs < rhs;
}

template <>
inline bool lsmap_less<double>::operator()(double lhs, double rhs) const {
	return lhs < rhs;
}

template <typename K, typename T>
LSValue* LSMap<K,T>::map_class(new LSClass("Map"));

template <typename K, typename T>
inline LSMap<K, T>::LSMap() {}

template <typename K, typename T>
inline LSMap<K,T>::~LSMap() {}

/*
 * Map methods
 */
template <>
inline LSMap<LSValue*,LSValue*>* LSMap<LSValue*,LSValue*>::ls_insert(const value_type& x) {
	emplace(x.first->clone(), x.second->clone());
	if (refs == 0) refs = 1;
	return this;
}
template <>
inline LSMap<LSValue*,int>* LSMap<LSValue*,int>::ls_insert(const value_type& x) {
	emplace(x.first->clone(), x.second);
	if (refs == 0) refs = 1;
	return this;
}
template <>
inline LSMap<LSValue*,double>* LSMap<LSValue*,double>::ls_insert(const value_type& x) {
	emplace(x.first->clone(), x.second);
	if (refs == 0) refs = 1;
	return this;
}
template <>
inline LSMap<int,LSValue*>* LSMap<int,LSValue*>::ls_insert(const value_type& x) {
	emplace(x.first, x.second->clone());
	if (refs == 0) refs = 1;
	return this;
}
template <>
inline LSMap<int,int>* LSMap<int,int>::ls_insert(const value_type& x) {
	insert(x);
	if (refs == 0) refs = 1;
	return this;
}
template <>
inline LSMap<int,double>* LSMap<int,double>::ls_insert(const value_type& x) {
	insert(x);
	if (refs == 0) refs = 1;
	return this;
}


template <>
inline LSMap<LSValue*,LSValue*>* LSMap<LSValue*,LSValue*>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_val(it->first);
		LSValue::delete_val(it->second);
	}
	clear();
	refs++;
	return this;
}
template <>
inline LSMap<LSValue*,int>* LSMap<LSValue*,int>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_val(it->first);
	}
	clear();
	refs++;
	return this;
}
template <>
inline LSMap<LSValue*,double>* LSMap<LSValue*,double>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_val(it->first);
	}
	clear();
	refs++;
	return this;
}
template <>
inline LSMap<int,LSValue*>* LSMap<int,LSValue*>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_val(it->second);
	}
	clear();
	refs++;
	return this;
}
template <>
inline LSMap<int,int>* LSMap<int,int>::ls_clear() {
	clear();
	refs++;
	return this;
}
template <>
inline LSMap<int,double>* LSMap<int,double>::ls_clear() {
	clear();
	refs++;
	return this;
}



/*
 * LSValue methods
 */

template <typename K, typename T>
inline bool LSMap<K,T>::isTrue() const {
	return not this->empty();
}

template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator + (const LSValue* value) const {
	return value->operator + (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator += (LSValue* value) {
	return value->operator += (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator / (const LSValue* value) const {
	return value->operator / (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::poww(const LSValue* value) const {
	return value->poww(this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}
template <typename K, typename T>
inline LSValue* LSMap<K,T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}
template <typename K, typename T>
inline bool LSMap<K,T>::operator == (const LSValue* value) const {
	return value->operator == (this);
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
	return os << "]";
}
template <>
inline std::ostream& LSMap<int,int>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : " << it->second;
	}
	return os << "]";
}
template <>
inline std::ostream& LSMap<int,double>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : " << it->second;
	}
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
		map->emplace(it->first->clone(), it->second->clone());
	}
	return map;
}
template <>
inline LSValue* LSMap<LSValue*,int>::clone() const {
	LSMap<LSValue*,int>* map = new LSMap<LSValue*,int>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone(), it->second);
	}
	return map;
}
template <>
inline LSValue* LSMap<LSValue*,double>::clone() const {
	LSMap<LSValue*,double>* map = new LSMap<LSValue*,double>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone(), it->second);
	}
	return map;
}
template <>
inline LSValue* LSMap<int,LSValue*>::clone() const {
	LSMap<int,LSValue*>* map = new LSMap<int,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first, it->second->clone());
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
