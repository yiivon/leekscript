#ifndef LS_SET_TCC
#define LS_SET_TCC

#include "LSSet.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"

namespace ls {

template <>
inline bool lsset_less<LSValue*>::operator()(LSValue* lhs, LSValue* rhs) const {
	return *lhs < *rhs;
}

template <typename T>
inline bool lsset_less<T>::operator()(T lhs, T rhs) const {
	return lhs < rhs;
}

template <typename T>
LSValue* LSSet<T>::clazz;


template <typename T>
LSSet<T>* LSSet<T>::constructor() {
	return new LSSet<T>();
}

template <class T>
inline LSSet<T>::LSSet() : LSValue(SET) {}

template <class T>
LSSet<T>::LSSet(std::initializer_list<T> values) : LSValue(SET), std::set<T, lsset_less<T>>(values) {}

template <>
inline LSSet<LSValue*>::LSSet(const LSSet<LSValue*>& other) : LSValue(other), std::set<LSValue*, lsset_less<LSValue*>>() {
	for (LSValue* v : other) {
		insert(end(), v->clone_inc());
	}
}

template <typename T>
inline LSSet<T>::LSSet(const LSSet<T>& other) : LSValue(other), std::set<T, lsset_less<T>>(other) {}

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
int LSSet<T>::ls_size() {
	int s = this->size();
	LSValue::delete_temporary(this);
	return s;
}

template <>
inline bool LSSet<LSValue*>::ls_insert(LSValue* value) {
	auto it = lower_bound(value);
	if (it == end() || (**it != *value)) {
		insert(it, value->move_inc());
		LSValue::delete_temporary(this);
		return true;
	}
	LSValue::delete_temporary(value);
	LSValue::delete_temporary(this);
	return false;
}

template <>
inline LSValue* LSSet<LSValue*>::ls_insert_ptr(LSValue* value) {
	return LSBoolean::get(ls_insert(value));
}

template <typename T>
inline bool LSSet<T>::ls_insert(T value) {
	bool r = this->insert(value).second;
	return r;
}

template <>
inline void LSSet<LSValue*>::vinsert(LSValue* value) {
	auto it = lower_bound(value);
	if (it == end() || (**it != *value)) {
		insert(it, value->move_inc());
	} else {
		LSValue::delete_temporary(value);
	}
}
template <typename T>
inline void LSSet<T>::vinsert(T value) {
	this->insert(value);
}

template <class T>
LSSet<T>* LSSet<T>::ls_clear() {
	for (T v : *this) {
		ls::unref(v);
	}
	this->clear();
	return this;
}

template <>
inline bool LSSet<LSValue*>::ls_erase(LSValue* value) {
	auto it = find(value);
	LSValue::delete_temporary(value);
	if (it == end()) {
		if (refs == 0) delete this;
		return false;
	} else {
		LSValue::delete_ref(*it);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
}
template <typename T>
inline bool LSSet<T>::ls_erase(T value) {
	bool r = this->erase(value);
	if (refs == 0) delete this;
	return r;
}

template <class T>
bool LSSet<T>::ls_contains(T value) {
	bool r = this->count(value);
	ls::release(value);
	if (refs == 0) delete this;
	return r;
}

template <typename T>
bool LSSet<T>::to_bool() const {
	return !this->empty();
}

template <typename T>
bool LSSet<T>::ls_not() const {
	auto r = this->empty();
	LSValue::delete_temporary(this);
	return r;
}

template <>
inline LSValue* LSSet<int>::add_eq(LSValue* v) {
	if (v->type == LSValue::NUMBER) {
		int vv = static_cast<LSNumber*>(v)->value;
		this->insert(this->end(), vv);
	}
	if (v->type == LSValue::SET) {
		if (auto s = dynamic_cast<LSSet<int>*>(v)) {
			this->insert(s->begin(), s->end());
		}
	}
	LSValue::delete_temporary(v);
	return this;
}

template <>
inline LSValue* LSSet<double>::add_eq(LSValue* v) {
	if (v->type == LSValue::NUMBER) {
		double vv = static_cast<LSNumber*>(v)->value;
		this->insert(this->end(), vv);
	}
	if (v->type == LSValue::SET) {
		if (auto s = dynamic_cast<LSSet<int>*>(v)) {
			this->insert(s->begin(), s->end());
		}
		if (auto s = dynamic_cast<LSSet<double>*>(v)) {
			this->insert(s->begin(), s->end());
		}
	}
	LSValue::delete_temporary(v);
	return this;
}

template <>
inline LSValue* LSSet<LSValue*>::add_eq(LSValue* v) {
	if (v->type == LSValue::SET) {
		if (auto s = dynamic_cast<LSSet<LSValue*>*>(v)) {
			for (auto e : *s)
				this->insert(this->end(), e->clone_inc());
		}
		if (auto s = dynamic_cast<LSSet<int>*>(v)) {
			for (auto e : *s) {
				auto n = LSNumber::get(e);
				n->refs = 1;
				this->insert(this->end(), n);
			}
		}
		if (auto s = dynamic_cast<LSSet<double>*>(v)) {
			for (auto e : *s) {
				auto n = LSNumber::get(e);
				n->refs = 1;
				this->insert(this->end(), n);
			}
		}
		LSValue::delete_temporary(v);
		return this;
	}
	this->insert(this->end(), v->move_inc());
	return this;
}

template <class T>
LSValue* LSSet<T>::add_eq_int(int) {
	// TODO
	return this;
}

template <>
inline LSValue* LSSet<int>::add_eq_int(int v) {
	insert(v);
	return this;
}

template <class T>
LSValue* LSSet<T>::add_eq_double(double) {
	// TODO
	return this;
}

template <>
inline LSValue* LSSet<double>::add_eq_double(double v) {
	insert(v);
	return this;
}

template <class T1, class T2>
bool set_equals(const LSSet<T1>* s1, const LSSet<T2>* s2) {
	if (s1->size() != s2->size()) return false;
	auto it1 = s1->begin();
	auto it2 = s2->begin();
	while (it1 != s1->end()) {
		if (!ls::equals(*it1, *it2)) return false;
		++it1;
		++it2;
	}
	return true;
}

template <class T>
bool LSSet<T>::eq(const LSValue* v) const {
	if (auto set = dynamic_cast<const LSSet<LSValue*>*>(v)) {
		return set_equals(this, set);
	}
	if (auto set = dynamic_cast<const LSSet<int>*>(v)) {
		return set_equals(this, set);
	}
	if (auto set = dynamic_cast<const LSSet<double>*>(v)) {
		return set_equals(this, set);
	}
	return false;
}

template <class T>
template <class T2>
bool LSSet<T>::set_lt(const LSSet<T2>* set) const {
	auto i = this->begin();
	auto j = set->begin();
	while (i != this->end()) {
		if (j == set->end())
			return false;
		if (ls::lt(*i, *j))
			return true;
		if (ls::lt(*j, *i))
			return false;
		++i;
		++j;
	}
	return j != set->end();
}


template <typename T>
LSSet<LSValue*>* LSSet<T>::to_any_set() const {
	auto result = new LSSet<LSValue*>();
	for (const auto& e : *this) {
		result->emplace(ls::convert<LSValue*>(e));
	}
	return result;
}

template <>
inline bool LSSet<LSValue*>::lt(const LSValue* v) const {
	if (auto set = dynamic_cast<const LSSet<LSValue*>*>(v)) {
		return std::lexicographical_compare(begin(), end(), set->begin(), set->end(), [](LSValue* a, LSValue* b) -> bool {
			return *a < *b;
		});
	}
	if (auto set = dynamic_cast<const LSSet<int>*>(v)) {
		return set_lt(set);
	}
	if (auto set = dynamic_cast<const LSSet<double>*>(v)) {
		return set_lt(set);
	}
	return LSValue::lt(v);
}

template <typename T>
inline bool LSSet<T>::lt(const LSValue* v) const {
	if (auto set = dynamic_cast<const LSSet<LSValue*>*>(v)) {
		return set_lt(set);
	}
	if (auto set = dynamic_cast<const LSSet<int>*>(v)) {
		return std::lexicographical_compare(this->begin(), this->end(), set->begin(), set->end());
	}
	if (auto set = dynamic_cast<const LSSet<double>*>(v)) {
		return std::lexicographical_compare(this->begin(), this->end(), set->begin(), set->end());
	}
	return LSValue::lt(v);
}

template <class T>
bool LSSet<T>::in(const LSValue* const value) const {
	bool r = false;
	LSValue::delete_temporary(this);
	ls::release(value);
	return r;
}

template <class T>
bool LSSet<T>::in_v(const T value) const {
	bool r = this->count(value);
	LSValue::delete_temporary(this);
	ls::release(value);
	return r;
}

template <class T>
int LSSet<T>::abso() const {
	return this->size();
}

template <typename T>
inline std::ostream& LSSet<T>::dump(std::ostream& os, int level) const {
	os << "<";
	if (level > 0) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if (i != this->begin()) os << ", ";
			os << *i;
		}
	} else {
		os << " ... ";
	}
	os << ">";
	return os;
}

template <typename T>
inline std::string LSSet<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ", ";
		std::string json = ls::to_json(*i);
		res += json;
	}
	return res + "]";
}

template <typename T>
inline LSValue* LSSet<T>::clone() const {
	return new LSSet<T>(*this);
}

template <typename T>
LSValue* LSSet<T>::getClass() const {
	return LSSet<T>::clazz;
}

}

#endif
