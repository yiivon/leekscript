#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include "LSMap.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"

#include <exception>

using namespace std;

namespace ls {

template <class K>
bool lsmap_less<K>::operator()(K lhs, K rhs) const {
	return rhs->operator < (lhs);
}

template <>
bool lsmap_less<int>::operator()(int lhs, int rhs) const {
	return lhs < rhs;
}

template <>
bool lsmap_less<double>::operator()(double lhs, double rhs) const {
	return lhs < rhs;
}

template <typename K, typename T>
LSValue* LSMap<K,T>::map_class(new LSClass("Map", 1));

template <typename K, typename T>
LSMap<K,T>::LSMap() {}

template <typename K, typename T>
LSMap<K,T>::LSMap(initializer_list<pair<LSValue*, T>> values) {
	for (auto i : values) {
		(*this)[i.first] = i.second;
	}
}

template <typename K, typename T>
LSMap<K,T>::~LSMap() {}

template <typename K, typename T>
bool LSMap<K,T>::isTrue() const {
	return !this->empty();
}

template <typename K, typename T>
LSValue* LSMap<K,T>::operator + (const LSValue* value) const {
	return value->operator + (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator += (LSValue* value) {
	return value->operator += (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator / (const LSValue* value) const {
	return value->operator / (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::poww(const LSValue* value) const {
	return value->poww(this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}
template <typename K, typename T>
LSValue* LSMap<K,T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}
template <typename K, typename T>
bool LSMap<K,T>::operator == (const LSValue* value) const {
	return value->operator == (this);
}
template <typename K, typename T>
bool LSMap<K,T>::operator <(const LSValue* value) const {
	return value->operator <(this);
}

template <>
LSValue* LSMap<LSValue*,LSValue*>::at(const LSValue* key) const {

	try {
		return (LSValue*) ((std::map<LSValue*,LSValue*>*) this)->at((LSValue*) key)->clone();
	} catch (std::exception&) {
		return LSNull::null_var;
	}
}
template <>
LSValue* LSMap<LSValue*,int>::at(const LSValue* key) const {

	try {
		return LSNumber::get(((std::map<LSValue*,int>*) this)->at((LSValue*) key));
	} catch (std::exception&) {
		return LSNull::null_var;
	}
}
template <>
LSValue* LSMap<LSValue*,double>::at(const LSValue* key) const {

	try {
		return LSNumber::get(((std::map<LSValue*,double>*) this)->at((LSValue*) key));
	} catch (std::exception&) {
		return LSNull::null_var;
	}
}
template <>
LSValue* LSMap<int,LSValue*>::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue*) ((std::map<int,LSValue*>*) this)->at((int) n->value)->clone();
		} catch (std::exception&) {
			return LSNull::null_var;
		}
	}
	return LSNull::null_var;
}
template <>
LSValue* LSMap<int,int>::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::map<int,int>*) this)->at((int) n->value));
		} catch (std::exception&) {
			return LSNull::null_var;
		}
	}
	return LSNull::null_var;
}
template <>
LSValue* LSMap<int,double>::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::map<int,double>*) this)->at((int) n->value));
		} catch (std::exception&) {
			return LSNull::null_var;
		}
	}
	return LSNull::null_var;
}


template <typename K, typename T>
LSValue** LSMap<K,T>::atL(const LSValue*) {
	return &LSNull::null_var;
}
template <>
LSValue** LSMap<LSValue*,LSValue*>::atL(const LSValue* key) {
	try {
		return (LSValue**) &((std::map<LSValue*,LSValue*>*) this)->at((LSValue*) key);
	} catch (std::exception&) {
		return &LSNull::null_var;
	}
}
template <>
LSValue** LSMap<int,LSValue*>::atL(const LSValue* key) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue**) &((std::map<int,LSValue*>*) this)->at((int) n->value);
		} catch (std::exception&) {
			return &LSNull::null_var;
		}
	}
	return &LSNull::null_var;
}

template <typename K, typename T>
int LSMap<K,T>::atv(const LSValue* key) {

}

template <typename K, typename T>
int* LSMap<K,T>::atLv(const LSValue* key) {

}

template <>
std::ostream& LSMap<LSValue*,LSValue*>::print(std::ostream& os) const {
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
std::ostream& LSMap<LSValue*,int>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		it->first->print(os);
		os << " : " << it->second;
	}
	return os << "]";
}
template <>
std::ostream& LSMap<LSValue*,double>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		it->first->print(os);
		os << " : " << it->second;
	}
	return os << "]";
}
template <>
std::ostream& LSMap<int,LSValue*>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : ";
		it->second->print(os);
	}
	return os << "]";
}
template <>
std::ostream& LSMap<int,int>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		os << it->first << " : " << it->second;
	}
	return os << "]";
}
template <>
std::ostream& LSMap<int,double>::print(std::ostream& os) const {
	os << "[";
	for (auto it = begin(); it != end(); ++it) {
		if (it != begin()) os << " ";
		it->first->print(os);
		os << it->first << " : " << it->second;
	}
	return os << "]";
}

template <typename K, typename T>
std::string LSMap<K,T>::json() const {
	return "this is a map";
}

template <>
LSValue* LSMap<LSValue*,LSValue*>::clone() const {
	LSMap<LSValue*,LSValue*>* map = new LSMap<LSValue*,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone(), it->second->clone());
	}
	return map;
}
template <>
LSValue* LSMap<LSValue*,int>::clone() const {
	LSMap<LSValue*,int>* map = new LSMap<LSValue*,int>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone(), it->second);
	}
	return map;
}
template <>
LSValue* LSMap<LSValue*,double>::clone() const {
	LSMap<LSValue*,double>* map = new LSMap<LSValue*,double>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone(), it->second);
	}
	return map;
}
template <>
LSValue* LSMap<int,LSValue*>::clone() const {
	LSMap<int,LSValue*>* map = new LSMap<int,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first, it->second->clone());
	}
	return map;
}
template <>
LSValue* LSMap<int,int>::clone() const {
	return new LSMap<int,int>(*this);
}
template <>
LSValue* LSMap<int,double>::clone() const {
	return new LSMap<int,double>(*this);
}

template <typename K, typename T>
LSValue* LSMap<K,T>::getClass() const {
	return LSMap<K,T>::map_class;
}

template <typename K, typename T>
int LSMap<K,T>::typeID() const {
	return 6;
}

template <typename K, typename T>
const BaseRawType* LSMap<K,T>::getRawType() const {
	return RawType::MAP;
}



}

#endif
