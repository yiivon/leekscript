#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include "LSNull.hpp"
#include "LSNumber.hpp"
#include "LSBoolean.hpp"
#include "LSFunction.hpp"

namespace ls {

template <class T>
LSValue* LSArray<T>::array_class(new LSClass("Array", 1));

template <class T>
void LSArray<T>::push_clone(const T value) {
	if (value->native) {
		this->push_back(value); // no clone if native value
	} else {
		T val = (T) value->clone();
		((LSValue*) val)->refs++;
		this->push_back(val);
	}
}

template <>
inline void LSArray<int>::push_clone(const int value) {
	this->push_back(value);
}

template <>
inline void LSArray<double>::push_clone(const double value) {
	this->push_back(value);
}

template <class T>
LSArray<T>::LSArray() {}

template <class T>
LSArray<T>::LSArray(std::initializer_list<T> values_list) {
	for (auto i : values_list) {
		this->push_back(i);
	}
}

template <class T>
LSArray<T>::LSArray(const std::vector<T>& vec) {
	for (auto i : vec) {
		this->push_back(i);
	}
}

template <class T>
LSArray<T>::LSArray(JsonValue& json) {
	for (auto e : json) {
		push_clone((T) LSValue::parse(e->value));
	}
}

template <class T>
LSArray<T>::~LSArray() {
//	std::cout << "~LSArray<T>" << std::endl;
	for (auto v : *this) {
//		std::cout << "delete ";
//		v->print(std::cout);
//		std::cout << " " << v->refs << std::endl;
		LSValue::delete_val((LSValue*) v);
	}
}

template <>
inline LSArray<int>::~LSArray() {
//	std::cout << "~LSArray<int>" << std::endl;
}
template <>
inline LSArray<double>::~LSArray() {
//	std::cout << "~LSArray<double>" << std::endl;
}

template <class T>
void LSArray<T>::clear() {
	((std::vector<T>*) this)->clear();
}

template <class T>
T LSArray<T>::remove(const int index) {
	T previous = this->operator [] (index);
	this->erase(this->begin() + index);
	return previous;
}

template <class T>
T LSArray<T>::remove_key(LSValue*) {
	// TODO : move all indices after index to the left ?
	// or transform the array into a associative one
	return LSNull::null_var;
}

template <class T>
T LSArray<T>::remove_element(T) {
	// TODO : move all indices after index to the left ?
	// or transform the array into a associative one
	return LSNull::null_var;
}

template <>
inline int LSArray<int>::remove_element(int element) {
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (*it == element) {
			this->erase(it);
			return element;
		}
	}
	return 0;
}

template <class T>
inline size_t LSArray<T>::size() const {
	return ((std::vector<T>*) this)->size();
}

template <class T>
T LSArray<T>::sum() const {
	if (size() == 0) return (T) LSNumber::get(0);
	LSValue* sum = this->operator [] (0)->clone();
	for (unsigned i = 1; i < this->size(); ++i) {
		LSValue* new_sum = this->operator [] (i)->operator + (sum);
		new_sum->refs++;
		LSValue::delete_val(sum);
		sum = new_sum;
	}
	return (T) sum;
}

template <>
inline int LSArray<int>::sum() const {
	int sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	return sum;
}

template <>
inline double LSArray<double>::sum() const {
	double sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	return sum;
}

template <class T>
double LSArray<T>::average() const {
	return 0; // No average for a no integer array
}

template <>
inline double LSArray<double>::average() const {
	if (size() == 0) return 0;
	return this->sum() / size();
}

template <>
inline double LSArray<int>::average() const {
	if (size() == 0) return 0;
	return (double) this->sum() / size();
}

template <class T>
T LSArray<T>::first() const {
	if (size() == 0) return T();
	return this->operator [] (0);
}

template <class T>
T LSArray<T>::last() const {
	if (size() == 0) return T();
	return this->back();
}

template <class T>
LSValue* LSArray<T>::pop() {
	LSValue* last = LSNull::null_var;
	if (this->size() > 0) {
		last = this->back();
		this->pop_back();
	}
	return last;
}

template <>
inline LSValue* LSArray<int>::pop() {
	if (this->size() > 0) {
		int last = this->back();
		this->pop_back();
		return LSNumber::get(last);
	}
	return LSNull::null_var;
}

template <class T>
void LSArray<T>::push_no_clone(T value) {
	this->push_back(value);
	value->refs++;
}

template <>
inline void LSArray<int>::push_no_clone(int value) {
	this->push_back(value);
}
template <>
inline void LSArray<double>::push_no_clone(double value) {
	this->push_back(value);
}

template <class T>
LSArray<LSValue*>* LSArray<T>::map(const void* function) const {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	auto fun = (void* (*)(void*)) function;

	for (auto v : *this) {
//		v->print(std::cout);
//		std::cout << std::endl;
		LSValue* res = (LSValue*) fun((void*) v);
//		res->print(std::cout);
//		std::cout << std::endl;
		new_array->push_clone(res);
	}
	return new_array;
}

template <class T>
void LSArray<T>::iter(const LSFunction* function) const {

	auto fun = (void* (*)(void*)) function->function;
	for (auto v : *this) {
		fun((void*) v);
	}
}

template <class T>
int LSArray<T>::contains(const LSValue* val) const {
	for (auto v : *this) {
		if (v->operator == (val)) {
			return true;
		}
	}
	return false;
}

template <>
inline int LSArray<int>::contains(const LSValue* val) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(val)) {
		for (auto v : *this) {
			if (v == n->value) return true;
		}
	}
	return false;
}

template <>
inline int LSArray<int>::contains_int(int val) const {
	for (auto v : *this) {
		if (v == val) return true;
	}
	return false;
}

template <class T>
LSArray<T>* LSArray<T>::push(const T val) {
	push_clone(val);
	if (this->refs == 0) refs = 1;
	return this;
}

template <class T>
LSArray<T>* LSArray<T>::push_all(const LSArray<LSValue*>* array) {
	for (auto v : *array) {
		push_clone((T) v);
	}
	if (this->refs == 0) refs = 1;
	return this;
}

template <>
inline const LSArray<int>* LSArray<int>::push_all_int(const LSArray<int>* array) {
	for (auto v : *array) {
		push_clone(v);
	}
	if (this->refs == 0) refs = 1;
	return this;
}

template <class T>
LSArray<T>* LSArray<T>::reverse() const {
	LSArray<T>* new_array = new LSArray<T>();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		new_array->push_clone(*it);
	}
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::reverse() const {
	LSArray<int>* new_array = new LSArray<int>();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		new_array->push_clone(*it);
	}
	return new_array;
}

template <class T>
LSArray<T>* LSArray<T>::filter(const void* function) const {
//	this->print(std::cout);
	LSArray<T>* new_array = new LSArray<T>();
	auto fun = (bool (*)(void*)) function;
	for (auto v : *this) {
		if (fun(v)) new_array->push_clone(v);
	}
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::filter(const void* function) const {
	LSArray<int>* new_array = new LSArray<int>();
	auto fun = (int (*)(int)) function;
	for (auto v : *this) {
		if (fun(v)) {
			new_array->push_clone(v);
		}
	}
	return new_array;
}

template <class T>
LSValue* LSArray<T>::foldLeft(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;
	LSValue* result = v0->clone();
	for (auto v : *this) {
		result = fun(result, v);
	}
	return result;
}

template <>
inline LSValue* LSArray<int>::foldLeft(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(LSValue*, int)) function;
	LSValue* result = v0->clone();
	for (auto v : *this) {
		result = fun(result, v);
	}
	return result;
}

template <class T>
LSValue* LSArray<T>::foldRight(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;
	LSValue* result = v0->clone();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(*it, result);
	}
	return result;
}

template <>
inline LSValue* LSArray<int>::foldRight(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(int, LSValue*)) function;
	LSValue* result = v0->clone();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(*it, result);
	}
	return result;
}

template <class T>
LSArray<T>* LSArray<T>::insert_v(const T v, const LSValue* pos) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(pos)) {
		if (this->size() <= n->value) {
			this->resize(n->value);
		}
		this->insert(this->begin() + (int) n->value, v);
	}
	return this;
}

template <>
inline LSArray<int>* LSArray<int>::insert_v(const int v, const LSValue* pos) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(pos)) {
		if (this->size() <= n->value) {
			this->resize(n->value);
		}
		this->insert(this->begin() + (int) n->value, (int) v);
	}
	return this;
}

template <class T>
LSArray<LSArray<T>*>* LSArray<T>::partition(const void* function) const {

	LSArray<T>* array_true = new LSArray<T>();
	LSArray<T>* array_false = new LSArray<T>();
	auto fun = (bool (*)(void*)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_clone(v);
		} else {
			array_false->push_clone(v);
		}
	}
	return new LSArray<LSArray<T>*> {array_true, array_false};
}

template <>
inline LSArray<LSArray<int>*>* LSArray<int>::partition(const void* function) const {

	LSArray<int>* array_true = new LSArray<int>();
	LSArray<int>* array_false = new LSArray<int>();
	auto fun = (bool (*)(int)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_clone(v);
		} else {
			array_false->push_clone(v);
		}
	}
	return new LSArray<LSArray<int>*> {array_true, array_false};
}

template <class T>
LSArray<LSValue*>* LSArray<T>::map2(const LSArray<LSValue*>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		LSValue* v2 = ((LSArray<LSValue*>*) array)->operator [] (i);
		new_array->push_clone((LSValue*) fun(v1, v2));
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::map2(const LSArray<LSValue*>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = LSNumber::get(this->operator [] (i));
		LSValue* v2 = array->operator [] (i);
		new_array->push_clone((LSValue*) fun(v1, v2));
	}
	return new_array;
}

template <class T>
LSArray<LSValue*>* LSArray<T>::map2_int(const LSArray<int>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	auto fun = (void* (*)(void*, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		new_array->push_clone((LSValue*) fun(v1, v2));
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::map2_int(const LSArray<int>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	auto fun = (void* (*)(int, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		int v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		new_array->push_clone((LSValue*) fun(v1, v2));
	}
	return new_array;
}

template <class T>
int LSArray<T>::search(const LSValue* value, const int start) const {

	for (unsigned i = 0; i < this->size(); i++) {
		if (i < (unsigned) start) continue; // i < start
		if (value->operator == (this->operator[] (i))) {
			return i;
		}
	}
	return -1;
}

template <>
inline int LSArray<int>::search(const LSValue* value, const int start) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(value)) {
		for (unsigned i = 0; i < this->size(); i++) {
			if (i < (unsigned) start) continue; // i < start
			if (n->value == this->operator[] (i)) {
				return i;
			}
		}
	}
	return -1;
}

template <>
inline int LSArray<int>::search_int(const int value, const int start) const {

	for (unsigned i = 0; i < this->size(); i++) {
		if (i < (unsigned) start) continue; // i < start
		if (value == (this->operator[] (i))) {
			return i;
		}
	}
	return -1;
}

template <class T>
LSString* LSArray<T>::join(const LSString* glue) const {
	if (this->empty()) {
		return new LSString();
	}
	auto it = this->begin();
	LSValue* result = (*it)->operator + (new LSString());
	for (it++; it != this->end(); it++) {
		LSValue* n = (*it)->operator + (glue->operator + (result));
		n->refs++;
		LSValue::delete_val(result);
		result = n;
	}
	return (LSString*) result;
}

template <>
inline LSString* LSArray<int>::join(const LSString* glue) const {
	if (this->empty()) {
		return new LSString();
	}
	unsigned i = 0;
	std::string result = std::to_string(this->operator[] (i));
	for (i++; i < this->size(); i++) {
		result = std::to_string(this->operator[] (i)) + *glue + result;
	}
	return new LSString(result);
}

template <class T>
LSArray<T>* LSArray<T>::fill(const LSValue* element, const int size) {
	this->clear();
	for (int i = 0; i < size; i++) {
		this->push_clone((LSValue*) element);
	}
	return this;
}

template <>
inline LSArray<int>* LSArray<int>::fill(const LSValue* element, const int size) {
	this->clear();
	for (int i = 0; i < size; i++) {
		this->push_clone(((LSNumber*) element)->value);
	}
	return this;
}


/*
 * LSValue methods
 */
template <class T>
bool LSArray<T>::isTrue() const {
	return this->size() > 0;
}

template <class T>
LSValue* LSArray<T>::operator - () const {
	return this->clone();
}

template <class T>
LSValue* LSArray<T>::operator ! () const {
	return LSBoolean::get(this->size() == 0);
}

template <class T>
LSValue* LSArray<T>::operator ~ () const {
	LSArray<T>* array = new LSArray<T>();
	for (auto i = this->rbegin(); i != this->rend(); ++i) {
		array->push_clone(*i);
	}
	return array;
}

template <class T>
LSValue* LSArray<T>::operator ++ () {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator ++ (int) {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator -- () {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator -- (int) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSValue* v) const {
	return v->operator + (this);
}

template <class T>
LSValue* LSArray<T>::operator + (const LSNull* v) const {
	LSArray* new_array = (LSArray*) clone();
	new_array->push_clone((T) v);
	return new_array;
}

template <>
inline LSValue* LSArray<int>::operator + (const LSNull*) const {
	return clone();
}

template <>
inline LSValue* LSArray<double>::operator + (const LSNull*) const {
	return clone();
}

template <class T>
LSValue* LSArray<T>::operator + (const LSBoolean*) const {
	LSArray* new_array = (LSArray*) clone();
//	new_array->push_clone(boolean);
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSNumber* v) const {
	LSArray* new_array = (LSArray*) clone();
	new_array->push_clone((T) v);
	return new_array;
}

template <>
inline LSValue* LSArray<int>::operator + (const LSNumber* number) const {
	LSArray* new_array = (LSArray*) clone();
	new_array->push_clone(number->value);
	return new_array;
}

template <>
inline LSValue* LSArray<double>::operator + (const LSNumber* number) const {
	LSArray* new_array = (LSArray*) clone();
	new_array->push_clone(number->value);
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSString*) const {
	LSArray* new_array = (LSArray*) clone();
//	new_array->push_clone(string);
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSArray<LSValue*>* array) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	for (auto v : *this) {
		new_array->push_clone((LSValue*) v);
	}
	for (auto v : *array) {
		new_array->push_clone(v);
	}
	return new_array;
}

template <>
inline LSValue* LSArray<int>::operator + (const LSArray<LSValue*>*) const {

	LSArray<int>* new_array = new LSArray<int>();
	return new_array;
}

template <>
inline LSValue* LSArray<double>::operator + (const LSArray<LSValue*>*) const {

	LSArray<int>* new_array = new LSArray<int>();
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSArray<int>*) const {

	LSArray<int>* new_array = new LSArray<int>();
	/*
	new_array->insert(new_array->end(), this->begin(), this->end());
	new_array->insert(new_array->end(), array->begin(), array->end());
	*/
	return new_array;
}

template <>
inline LSValue* LSArray<int>::operator + (const LSArray<int>* array) const {

	LSArray<int>* new_array = new LSArray<int>();
	new_array->insert(new_array->end(), this->begin(), this->end());
	new_array->insert(new_array->end(), array->begin(), array->end());
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSObject*) const {
	LSArray* new_array = (LSArray*) clone();
//	new_array->push_clone(object);
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSFunction*) const {
	LSArray* new_array = (LSArray*) clone();
//	new_array->push_clone(fun);
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator + (const LSClass*) const {
	LSArray* new_array = (LSArray*) clone();
//	new_array->push_clone(clazz);
	return new_array;
}

template <class T>
LSValue* LSArray<T>::operator += (LSValue* value) {
	return value->operator += (this);
}

template <class T>
LSValue* LSArray<T>::operator += (const LSNull*) {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSBoolean*) {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSNumber*) {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSString*) {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSArray<LSValue*>* array) {

	LSArray* arr = (LSArray*) array;

	if (array == (void*)this) {
		arr = (LSArray*) array->clone();
	}
	for (auto i = arr->begin(); i != arr->end(); ++i) {
		push_clone(*i);
	}
	if (array == (void*)this) {
		delete arr;
	}
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSObject*) {
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSFunction*) {
//	pushClone((T) function);
	return this;
}

template <class T>
LSValue* LSArray<T>::operator += (const LSClass*) {
//	pushClone((T) clazz);
	return this;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}

template <class T>
LSValue* LSArray<T>::operator - (const LSNull*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSNumber* number) const {

	LSArray<T>* copy = (LSArray<T>*) clone();

	for (auto i = copy->begin(); i != copy->end(); ++i) {
		if ((*i)->operator == (number)) {
			copy->erase(i);
		}
	}
	return copy;
}

template <>
inline LSValue* LSArray<int>::operator - (const LSNumber*) const {
	LSArray<int>* copy = (LSArray<int>*) clone();
	return copy;
}

template <>
inline LSValue* LSArray<double>::operator - (const LSNumber*) const {
	LSArray<int>* copy = (LSArray<int>*) clone();
	return copy;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSString*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSObject*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSFunction*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator - (const LSClass*) const {
	return LSNull::null_var;
}


template <class T>
LSValue* LSArray<T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSNull*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSNumber*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSString*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSObject*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSFunction*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator -= (const LSClass*) {
	return LSNull::null_var;
}


template <class T>
LSValue* LSArray<T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}

template <class T>
LSValue* LSArray<T>::operator * (const LSNull*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSNumber*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSString*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSObject*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSFunction*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator * (const LSClass*) const {
	return LSNull::null_var;
}


template <class T>
LSValue* LSArray<T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSNull*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSNumber*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSString*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSObject*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSFunction*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator *= (const LSClass*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSValue* value) const {
	return value->operator / (value);
}

template <class T>
LSValue* LSArray<T>::operator / (const LSNull*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSNumber*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSString*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSObject*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSFunction*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator / (const LSClass*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSNull*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSNumber*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSString*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSObject*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSFunction*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator /= (const LSClass*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSValue* value) const {
	return value->poww(this);
}

template <class T>
LSValue* LSArray<T>::poww(const LSNull*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSBoolean*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSNumber*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSString*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSObject*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSFunction*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::poww(const LSClass*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSNull*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSString*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSArray<LSValue*>*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSObject*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}

template <class T>
LSValue* LSArray<T>::operator % (const LSNull*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSNumber*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSString*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSObject*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSFunction*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator % (const LSClass*) const {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSNull*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSNumber*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSString*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSObject*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSFunction*) {
	return LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::operator %= (const LSClass*) {
	return LSNull::null_var;
}

template <class T>
bool LSArray<T>::operator == (const LSValue* v) const {
	return v->operator == (this);
}

template <class T>
bool LSArray<T>::operator == (const LSNull* v) const {
	return v->operator == (this);
}

template <class T>
bool LSArray<T>::operator == (const LSBoolean*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator == (const LSNumber*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator == (const LSString*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator == (const LSArray<LSValue*>* v) const {

	if (this->size() != v->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = v->begin();

	for (; i != this->end(); i++, j++) {
		if ((*i)->operator != (*j)) return false;
	}
	return true;
}

template <>
inline bool LSArray<int>::operator == (const LSArray<LSValue*>* v) const {

	if (this->size() != v->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = v->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*j);
		if (!n) return false;
		if (n->value != *i) return false;
	}
	return true;
}

template <>
inline bool LSArray<double>::operator == (const LSArray<LSValue*>* v) const {

	if (this->size() != v->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = v->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*j);
		if (!n) return false;
		if (n->value != *i) return false;
	}
	return true;
}

template <class T>
bool LSArray<T>::operator == (const LSObject*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator == (const LSFunction*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator == (const LSClass*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator < (const LSValue* v) const {
	return v->operator < (this);
}

template <class T>
bool LSArray<T>::operator < (const LSNull*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator < (const LSBoolean*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator < (const LSNumber*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator < (const LSString*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator < (const LSArray<LSValue*>* v) const {
	return this->size() < v->size();
}

template <class T>
bool LSArray<T>::operator < (const LSObject*) const {
	return true;
}

template <class T>
bool LSArray<T>::operator < (const LSFunction*) const {
	return true;
}

template <class T>
bool LSArray<T>::operator < (const LSClass*) const {
	return true;
}

template <class T>
bool LSArray<T>::operator > (const LSValue* v) const {
	return v->operator > (this);
}
template <class T>
bool LSArray<T>::operator > (const LSNull*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator > (const LSBoolean*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator > (const LSNumber*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator > (const LSString*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator > (const LSArray<LSValue*>* v) const {
	return this->size() > v->size();
}
template <class T>
bool LSArray<T>::operator > (const LSObject*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator > (const LSFunction*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator > (const LSClass*) const {
	return false;
}

template <class T>
bool LSArray<T>::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
template <class T>
bool LSArray<T>::operator <= (const LSNull*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator <= (const LSBoolean*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator <= (const LSNumber*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator <= (const LSString*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator <= (const LSArray<LSValue*>* v) const {
	return this->size() <= v->size();
}
template <class T>
bool LSArray<T>::operator <= (const LSObject*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator <= (const LSFunction*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator <= (const LSClass*) const {
	return true;
}

template <class T>
bool LSArray<T>::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
template <class T>
bool LSArray<T>::operator >= (const LSNull*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator >= (const LSBoolean*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator >= (const LSNumber*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator >= (const LSString*) const {
	return true;
}
template <class T>
bool LSArray<T>::operator >= (const LSArray<LSValue*>* v) const {
	return this->size() >= v->size();
}
template <class T>
bool LSArray<T>::operator >= (const LSObject*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator >= (const LSFunction*) const {
	return false;
}
template <class T>
bool LSArray<T>::operator >= (const LSClass*) const {
	return false;
}

template <class T>
bool LSArray<T>::in(const LSValue* key) const {
	for (auto i = this->begin(); i != this->end(); i++) {
		if ((*i)->operator == (key)) {
			return true;
		}
	}
	return false;
}

template <>
inline bool LSArray<int>::in(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if ((*i) == n->value) {
				return true;
			}
		}
	}
	return false;
}

template <>
inline bool LSArray<double>::in(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if ((*i) == n->value) {
				return true;
			}
		}
	}
	return false;
}

template <>
inline int LSArray<int>::atv(int i) {
	return this->operator[] (i);
}

template <>
inline int* LSArray<int>::atLv(int i) {
	return &this->operator[] (i);
}

template <class T>
LSValue* LSArray<T>::range(int start, int end) const {

	LSArray<T>* range = new LSArray<T>();

	unsigned start_i = std::max(0, (int) start);
	unsigned end_i = std::min((int) size() - 1, (int) end);

	for (unsigned i = start_i; i <= end_i; i++) {
		range->push_clone(this->operator [] (i));
	}
	return range;
}

template <>
inline LSValue* LSArray<int>::range(int start, int end) const {

	LSArray<int>* range = new LSArray<int>();

	unsigned start_i = std::max(0, (int) start);
	unsigned end_i = std::min((int) size() - 1, (int) end);

	for (unsigned i = start_i; i <= end_i; i++) {
		range->push_no_clone(this->operator [] (i));
	}
	return range;
}

template <class T>
LSValue* LSArray<T>::rangeL(int, int) {
	return this;
}


template <class T>
LSValue* LSArray<T>::clone() const {

	LSArray<T>* new_array = new LSArray<T>();

	for (auto i = this->begin(); i != this->end(); i++) {
		new_array->push_clone(*i);
	}
	return new_array;
}

template <class T>
std::ostream& LSArray<T>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		(*i)->print(os);
//		os << " " << *i;
//		os << " " << (*i)->refs;
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<int>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<double>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <class T>
std::string LSArray<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = (*i)->to_json();
		res += json;
	}
	return res + "]";
}

template <>
inline std::string LSArray<int>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = std::to_string(*i);
		res += json;
	}
	return res + "]";
}

template <>
inline std::string LSArray<double>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = std::to_string(*i);
		res += json;
	}
	return res + "]";
}

template <class T>
LSValue* LSArray<T>::getClass() const {
	return LSArray<T>::array_class;
}

template <class T>
int LSArray<T>::typeID() const {
	return 5;
}

template <class T>
const BaseRawType* LSArray<T>::getRawType() const {
	return RawType::ARRAY;
}

template <>
inline LSArray<int>::LSArray(JsonValue& json) {
	for (auto e : json) {
		push_clone(((LSNumber*) LSValue::parse(e->value))->value);
	}
}


template <class T>
LSValue* LSArray<T>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue*) this->operator[] ((int) n->value);
		} catch (std::exception& e) {
			return LSNull::null_var;
		}
	}
	return LSNull::null_var;
}

template <>
inline LSValue* LSArray<int>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(this->operator[] ((int) n->value));
		} catch (std::exception& e) {
			return LSNull::null_var;
		}
	}
	return LSNull::null_var;
}

template <>
inline LSValue* LSArray<double>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(this->operator[] ((int) n->value));
		} catch (std::exception& e) {
			return LSNull::null_var;
		}
	}
	return LSNull::null_var;
}

template <>
inline LSValue* LSArray<int>::operator += (const LSNumber* number) {
	push_clone(number->value);
	return this;
}


template <>
inline LSArray<LSValue*>* LSArray<int>::map(const void* function) const {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	auto fun = (void* (*)(void*)) function;

	for (auto v : *this) {
		new_array->push_no_clone((LSValue*) fun((void*) LSNumber::get(v)));
	}
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::push_all(const LSArray<LSValue*>* array) {
	for (auto v : *array) {
		if (const LSNumber* n = dynamic_cast<const LSNumber*>(v)) {
			push_clone(n->value);
		}
	}
	return this;
}

template <class T>
const LSArray<T>* LSArray<T>::push_all_int(const LSArray<int>* array) {
	for (auto v : *array) {
		push_clone((T) LSNumber::get(v));
	}
	return this;
}

template <class T>
LSValue* LSArray<T>::attr(const LSValue* key) const {

	if (key->operator == (new LSString("size"))) {
		return LSNumber::get(this->size());
	}
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::null_var;
}


template <class T>
LSValue** LSArray<T>::atL(const LSValue* key) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			LSValue** v = (LSValue**) &(((std::vector<T>*)this)->at((int) n->value));
			return v;
		} catch (std::exception& e) {
			return &LSNull::null_var;
		}
	}
	return &LSNull::null_var;
}

template <class T>
LSValue** LSArray<T>::attrL(const LSValue*) {
	return &LSNull::null_var;
}

template <class T>
LSValue* LSArray<T>::abso() const {
	return LSNumber::get(size());
}

} // end of namespace ls

#endif

