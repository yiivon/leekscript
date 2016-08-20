#ifndef LS_SET_TCC
#define LS_SET_TCC

#include "LSSet.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"

namespace ls {

template <>
inline bool lsset_less<LSValue*>::operator()(LSValue* lhs, LSValue* rhs) const {
	return rhs->operator < (lhs);
}

template <typename T>
inline bool lsset_less<T>::operator()(T lhs, T rhs) const {
	return lhs < rhs;
}

template <typename T>
LSValue* LSSet<T>::set_class(new LSClass("Set"));

template <>
inline LSSet<LSValue*>::LSSet() {}
template <>
inline LSSet<double>::LSSet() {}
template <>
inline LSSet<int>::LSSet() {}

template <>
inline LSSet<LSValue*>::LSSet(const LSSet<LSValue*>& other) : LSValue(other), std::set<LSValue*, lsset_less<LSValue*>>() {
	for (LSValue* v : other) {
		insert(end(), v->clone_inc());
	}
}
template <typename T>
inline LSSet<T>::LSSet(const LSSet<T>& other) : LSValue(other), std::set<T, lsset_less<T>>(other) {
}


template <>
inline LSSet<LSValue*>::~LSSet() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(*it);
	}
}
template <typename T>
inline LSSet<T>::~LSSet() {
}

template <typename T>
bool LSSet<T>::ls_insert(T value)
{

}

template <typename T>
LSSet<T> *LSSet<T>::ls_clear()
{

}

template <typename T>
bool LSSet<T>::ls_erase(T value)
{

}

template <typename T>
bool LSSet<T>::ls_contains(T value)
{

}

template <typename T>
bool LSSet<T>::isTrue() const {
	return !this->empty();
}

template <typename T>
inline LSValue* LSSet<T>::operator + (const LSValue* value) const {
	return value->operator + (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator += (LSValue* value) {
	return value->operator += (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator / (const LSValue* value) const {
	return value->operator / (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}
template <typename T>
inline LSValue* LSSet<T>::poww(const LSValue* value) const {
	return value->poww(this);
}
template <typename T>
inline LSValue* LSSet<T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
template <typename T>
inline LSValue* LSSet<T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}
template <typename T>
inline LSValue* LSSet<T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}
template <typename T>
inline bool LSSet<T>::operator == (const LSValue* value) const {
	return value->operator == (this);
}


template <>
inline bool LSSet<LSValue*>::operator == (const LSSet<LSValue*>* other) const {
	if (size() != other->size()) return false;
	auto it1 = begin();
	auto it2 = other->begin();
	while (it1 != end()) {
		if ((*it1)->operator !=(*it2)) return false;
		++it1;
		++it2;
	}
	return true;
}
template <typename T>
inline bool LSSet<T>::operator == (const LSSet<LSValue*>* other) const {
	if (this->size() != other->size()) return false;
	auto it1 = this->begin();
	auto it2 = other->begin();
	while (it1 != this->end()) {
		LSNumber* v2 = dynamic_cast<LSNumber*>(*it2);
		if (!v2) return false;
		if (*it1 != v2->value) return false;
		++it1;
		++it2;
	}
	return true;
}

template <>
inline bool LSSet<LSValue*>::operator == (const LSSet<int>* other) const {
	if (size() != other->size()) return false;
	auto it1 = begin();
	auto it2 = other->begin();
	while (it1 != end()) {
		LSNumber* v1 = dynamic_cast<LSNumber*>(*it1);
		if (!v1) return false;
		if (*it2 != v1->value) return false;
		++it1;
		++it2;
	}
	return true;
}
template <typename T>
inline bool LSSet<T>::operator == (const LSSet<int>* other) const {
	if (this->size() != other->size()) return false;
	auto it1 = this->begin();
	auto it2 = other->begin();
	while (it1 != this->end()) {
		if (*it1 != *it2) return false;
		++it1;
		++it2;
	}
	return true;
}

template <>
inline bool LSSet<LSValue*>::operator == (const LSSet<double>* other) const {
	if (size() != other->size()) return false;
	auto it1 = begin();
	auto it2 = other->begin();
	while (it1 != end()) {
		LSNumber* v1 = dynamic_cast<LSNumber*>(*it1);
		if (!v1) return false;
		if (*it2 != v1->value) return false;
		++it1;
		++it2;
	}
	return true;
}
template <typename T>
inline bool LSSet<T>::operator == (const LSSet<double>* other) const {
	if (this->size() != other->size()) return false;
	auto it1 = this->begin();
	auto it2 = other->begin();
	while (it1 != this->end()) {
		if (*it1 != *it2) return false;
		++it1;
		++it2;
	}
	return true;
}

template <typename T>
inline bool LSSet<T>::operator <(const LSValue* value) const {
	return value->operator <(this);
}

template <typename T>
LSValue* LSSet<T>::at(const LSValue* ) const {
	return LSNull::get();
}

template <typename T>
LSValue** LSSet<T>::atL(const LSValue* ) {
	return nullptr;
}

template <>
inline std::ostream& LSSet<LSValue*>::print(std::ostream& os) const {
	os << "<";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		(*i)->print(os);
	}
	os << ">";
	return os;
}
template <typename T>
inline std::ostream& LSSet<T>::print(std::ostream& os) const {
	os << "<";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << *i;
	}
	os << ">";
	return os;
}

template <>
inline std::string LSSet<LSValue*>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = (*i)->to_json();
		res += json;
	}
	return res + "]";
}
template <typename T>
inline std::string LSSet<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = std::to_string(*i);
		res += json;
	}
	return res + "]";
}

template <typename T>
inline LSValue* LSSet<T>::clone() const {
	return new LSSet<T>(*this);
}

template <typename T>
LSValue*LSSet<T>::getClass() const {
	return LSSet<T>::set_class;
}

template <typename T>
int LSSet<T>::typeID() const {
	return 7;
}

template <typename T>
const BaseRawType*LSSet<T>::getRawType() const {
	return RawType::SET;
}


}

#endif
