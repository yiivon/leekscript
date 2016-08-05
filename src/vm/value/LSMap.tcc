#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include "LSMap.hpp"

#include <exception>

using namespace std;

namespace ls {

template <class T>
LSMap<T>::LSMap() {}

template <class T>
LSMap<T>::LSMap(initializer_list<pair<LSValue*, T>> values) {
	cout << "init LSMap" << endl;
	for (auto i : values) {
		(*this)[i.first] = i.second;
	}
}

template <class T>
LSMap<T>::~LSMap() {}


template <class T>
LSValue* LSMap<T>::operator + (const LSValue* v) const {
	return v->operator + (this);
}
template <class T>
LSValue* LSMap<T>::operator += (LSValue* value) {
	return value->operator += (this);
}
template <class T>
LSValue* LSMap<T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}
template <class T>
LSValue* LSMap<T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}
template <class T>
LSValue* LSMap<T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}
template <class T>
LSValue* LSMap<T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}
template <class T>
LSValue* LSMap<T>::operator / (const LSValue* value) const {
	return value->operator / (this);
}
template <class T>
LSValue* LSMap<T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}
template <class T>
LSValue* LSMap<T>::poww(const LSValue* value) const {
	return value->poww(this);
}
template <class T>
LSValue* LSMap<T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
template <class T>
LSValue* LSMap<T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}
template <class T>
LSValue* LSMap<T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}
template <class T>
bool LSMap<T>::operator == (const LSValue* v) const {
	return v->operator == (this);
}

}

#endif
