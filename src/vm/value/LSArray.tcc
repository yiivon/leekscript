#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include "LSNull.hpp"
#include "LSNumber.hpp"
#include "LSBoolean.hpp"
#include "LSFunction.hpp"

#include <algorithm>

namespace ls {

template <class T>
LSValue* LSArray<T>::array_class(new LSClass("Array"));

template <>
inline void LSArray<LSValue*>::push_clone(LSValue* value) {
	this->push_back(value->clone_inc());
}
template <>
inline void LSArray<int>::push_clone(int value) {
	this->push_back(value);
}
template <>
inline void LSArray<double>::push_clone(double value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_move(LSValue* value) {
	this->push_back(value->move_inc());
}
template <>
inline void LSArray<int>::push_move(int value) {
	this->push_back(value);
}
template <>
inline void LSArray<double>::push_move(double value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_no_clone(LSValue* value) {
	if (!value->native()) value->refs++;
	this->push_back(value);
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
LSArray<T>::LSArray(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone((T) LSValue::parse(it.value()));
	}
}

template <>
inline LSArray<int>::LSArray(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone(((LSNumber*) LSValue::parse(it.value()))->value);
	}
}

template <>
inline LSArray<LSValue*>::~LSArray() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
}
template <>
inline LSArray<int>::~LSArray() {
}
template <>
inline LSArray<double>::~LSArray() {
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_clear() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
	this->clear();
	return this;
}
template <class T>
inline LSArray<T>* LSArray<T>::ls_clear() {
	this->clear();
	return this;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_remove(int index) {
	LSValue* previous = this->operator [] (index);
	this->erase(this->begin() + index);
	previous->refs--;
	if (refs == 0) delete this;
	return previous;
}
template <typename T>
inline T LSArray<T>::ls_remove(int index) {
	T previous = this->operator [] (index);
	this->erase(this->begin() + index);
	if (refs == 0) delete this;
	return previous;
}

template <>
inline bool LSArray<LSValue*>::ls_remove_element(LSValue* element) {
	for (size_t i = 0; i < this->size(); ++i) {
		if (this->operator[] (i)->operator ==(element)) {
			LSValue::delete_temporary(element);
			LSValue::delete_ref(this->operator[] (i));
			(*this)[i] = this->back();
			this->pop_back();
			if (refs == 0) delete this;
			return true;
		}
	}
	LSValue::delete_temporary(element);
	if (refs == 0) delete this;
	return false;
}
template <typename T>
inline bool LSArray<T>::ls_remove_element(T element) {
	for (size_t i = 0; i < this->size(); ++i) {
		if ((*this)[i] == element) {
			(*this)[i] = this->back();
			this->pop_back();
			if (refs == 0) delete this;
			return true;
		}
	}
	if (refs == 0) delete this;
	return false;
}



template <>
inline LSValue* LSArray<LSValue*>::ls_sum() {
	if (this->size() == 0) return LSNumber::get(0);
	LSValue* sum = this->operator [] (0)->clone();
	for (unsigned i = 1; i < this->size(); ++i) {
		LSValue* new_sum = (*this)[i]->operator + (sum);
		LSValue::delete_temporary(sum);
		sum = new_sum;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline int LSArray<int>::ls_sum() {
	int sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline double LSArray<double>::ls_sum() {
	double sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline double LSArray<LSValue*>::ls_average() {
	if (refs == 0) delete this;
	return 0; // No average for a no integer array
}

template <>
inline double LSArray<double>::ls_average() {
	if (size() == 0) return 0;
	return this->ls_sum() / size();
}

template <>
inline double LSArray<int>::ls_average() {
	if (size() == 0) return 0;
	return (double) this->ls_sum() / size();
}

template <>
inline LSValue* LSArray<LSValue*>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* first = front();
	if (refs == 0) {
		if (first->refs == 1) {
			(*this)[0] = nullptr;
			first->refs = 0;
		}
		delete this;
		// In that case `first` will survive
	}
	return first->move(); /* return temporary */
}
template <>
inline LSValue* LSArray<double>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double first = front();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(first);
}
template <>
inline LSValue* LSArray<int>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double first = front();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(first);
}

template <>
inline LSValue* LSArray<LSValue*>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = back();
	if (refs == 0) {
		if (last->refs == 1) {
			pop_back();
			last->refs = 0;
		}
		delete this;
	}
	return last->move();
}
template <>
inline LSValue* LSArray<double>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double last = back();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(last);
}
template <>
inline LSValue* LSArray<int>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double last = back();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(last);
}

template <typename T>
inline bool LSArray<T>::ls_empty() {
	bool e = this->empty();
	if (refs == 0) delete this;
	return e;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = back();
	last->refs--;
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last->move();
}
template <>
inline LSValue* LSArray<int>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = LSNumber::get(back());
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last;
}
template <>
inline LSValue* LSArray<double>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = LSNumber::get(back());
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last;
}

template <typename T>
inline int LSArray<T>::ls_size() {
	int s = this->size();
	if (refs == 0) {
		delete this;
	}
	return s;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_map(const void* function) {

	auto fun = (LSValue* (*)(void*)) function;

	if (refs == 0) {
		/* In that case we have the abolute ownership of `this`
		 * Then instead of commit sucide, let optimize and work inplace
		 */
		for (size_t i = 0; i < size(); ++i) {
			LSValue* v = fun((*this)[i]);
			LSValue::delete_ref((*this)[i]);
			(*this)[i] = v->move_inc();
		}
		return this;
	} else {
		LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
		new_array->reserve(size());
		for (auto v : *this) {
			LSValue* c = v->clone();
			new_array->push_move(fun(c));
		}
		return new_array;
	}
}

template <>
inline LSArray<LSValue*>* LSArray<int>::ls_map(const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(int)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	if (refs == 0) {
		/* No other possibilities than sucide
		 * We have to delete temporary arguments that are not returned
		 */
		delete this;
	}
	return new_array;
}
template <>
inline LSArray<LSValue*>* LSArray<double>::ls_map(const void* function) {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(double)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}

template <>
inline LSArray<double>* LSArray<int>::ls_map_real(const void* function) {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(int)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}
template <>
inline LSArray<double>* LSArray<double>::ls_map_real(const void* function) {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(double)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::ls_map_int(const void* function) {
	auto fun = (int (*)(int)) function;

	if (refs == 0) {
		for (size_t i = 0; i < size(); ++i) {
			(*this)[i] = fun((*this)[i]);
		}
		return this;
	} else {
		LSArray<int>* new_array = new LSArray<int>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			new_array->push_back(fun(v));
		}
		return new_array;
	}
}
template <>
inline LSArray<int>* LSArray<double>::ls_map_int(const void* function) {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->reserve(this->size());
	auto fun = (int (*)(double)) function;
	for (auto v : *this) {
		new_array->push_no_clone(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}



template <typename T>
inline LSArray<LSValue*>* LSArray<T>::ls_chunk(int size) {
	if (size <= 0) size = 1;

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();

	new_array->reserve(this->size() / size + 1);

	size_t i = 0;
	while (i < this->size()) {
		LSArray<T>* sub_array = new LSArray<T>();
		sub_array->reserve(size);

		size_t j = std::min(i + size, this->size());
		if (refs == 0) {
			for (; i < j; ++i) {
				sub_array->push_no_clone((*this)[i]);
			}
		} else {
			for (; i < j; ++i) {
				sub_array->push_clone((*this)[i]);
			}
		}

		new_array->push_no_clone(sub_array);
	}
	if (refs == 0) {
		delete this;
	}
	return new_array;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_unique() {
	if (this->empty()) return this;

	auto it = this->begin();
	auto next = it;

	while (true) {
		++next;
		while (next != this->end() && (*next)->operator == (*it)) {
			LSValue::delete_ref(*next);
			next++;
		}
		++it;
		if (next == this->end()) {
			break;
		}
		*it = *next;
	}
	this->resize(std::distance(this->begin(), it));
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	return this;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_sort() {
	std::sort(this->begin(), this->end(), [](LSValue* a, LSValue* b) -> bool {
		return b->operator < (a);
	});
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_sort() {
	std::sort(this->begin(), this->end());
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_sort() {
	std::sort(this->begin(), this->end());
	return this;
}

template <class T>
void LSArray<T>::ls_iter(const void* function) {

	auto fun = (void* (*)(T)) function;

	for (auto v : *this) {
		fun(v);
	}

	if (refs == 0) delete this;
}

template <>
inline bool LSArray<LSValue*>::ls_contains(LSValue* val) {
	for (auto v : *this) {
		if (v->operator == (val)) {
			if (refs == 0) delete this;
			if (val->refs == 0) delete val;
			return true;
		}
	}
	if (refs == 0) delete this;
	if (val->refs == 0) delete val;
	return false;
}
template <>
inline bool LSArray<double>::ls_contains(double val) {
	for (auto v : *this) {
		if (v == val) {
			if (refs == 0) delete this;
			return true;
		}
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSArray<int>::ls_contains(int val) {
	for (auto v : *this) {
		if (v == val) {
			if (refs == 0) delete this;
			return true;
		}
	}
	if (refs == 0) delete this;
	return false;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_push(LSValue* val) {
	this->push_back(val->move_inc());
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_push(int val) {
	this->push_back(val);
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_push(double val) {
	this->push_back(val);
	return this;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all(LSArray<LSValue*>* array) {

	this->reserve(this->size() + array->size());

	if (array->refs == 0) {
		for (size_t i = 0; i < array->size(); ++i) {
			this->push_back((*array)[i]);
		}
		array->clear();
		delete array;
	} else {
		for (size_t i = 0; i < array->size(); ++i) {
			this->push_clone((*array)[i]);
		}
	}

	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all(LSArray<T>* array) {

	this->reserve(this->size() + array->size());

	for (size_t i = 0; i < array->size(); ++i) {
		this->push_back((*array)[i]);
	}
	if (array->refs == 0) delete array;
	return this;
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_shuffle() {

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ++i) {
			size_t j = rand() % this->size();
			T tmp = (*this)[i];
			(*this)[i] = (*this)[j];
			(*this)[j] = tmp;
		}
		return this;
	} else {
		LSArray<T>* new_array = new LSArray<T>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			new_array->push_clone(v);
		}
		for (size_t i = 0; i < new_array->size(); ++i) {
			size_t j = rand() % new_array->size();
			T tmp = (*new_array)[i];
			(*new_array)[i] = (*new_array)[j];
			(*new_array)[j] = tmp;
		}
		return new_array;
	}
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_reverse() {

	if (refs == 0) {
		for (size_t i = 0, j = this->size(); i < j; ++i, --j) {
			T tmp = (*this)[i];
			(*this)[i] = (*this)[j-1];
			(*this)[j-1] = tmp;
		}
		return this;
	} else {
		LSArray<T>* new_array = new LSArray<T>();
		new_array->reserve(this->size());
		for (auto it = this->rbegin(); it != this->rend(); it++) {
			new_array->push_clone(*it);
		}
		return new_array;
	}
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_filter(const void* function) {
	auto fun = (bool (*)(void*)) function;

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ) {
			LSValue* v = (*this)[i];
			if (!fun(v)) {
				LSValue::delete_ref(v);
				(*this)[i] = this->back();
				this->pop_back();
			} else {
				++i;
			}
		}
		return this;
	} else {
		LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}
template <>
inline LSArray<double>* LSArray<double>::ls_filter(const void* function) {
	auto fun = (bool (*)(double)) function;

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ) {
			double v = (*this)[i];
			if (!fun(v)) {
				(*this)[i] = this->back();
				this->pop_back();
			} else {
				++i;
			}
		}
		return this;
	} else {
		LSArray<double>* new_array = new LSArray<double>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}
template <>
inline LSArray<int>* LSArray<int>::ls_filter(const void* function) {
	auto fun = (bool (*)(int)) function;

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ) {
			int v = (*this)[i];
			if (!fun(v)) {
				(*this)[i] = this->back();
				this->pop_back();
			} else {
				++i;
			}
		}
		return this;
	} else {
		LSArray<int>* new_array = new LSArray<int>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}


template <>
inline LSValue* LSArray<LSValue*>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<int>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, int)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<double>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, double)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<int>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, int)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<double>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, double)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}



template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_insert(LSValue* value, int pos) {

	if (pos >= (int) this->size()) {
		this->resize(pos, LSNull::get());
	}

	this->insert(this->begin() + pos, value->move_inc());

	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_insert(T value, int pos) {

	if (pos >= (int) this->size()) {
		this->resize(pos, (T) 0);
	}

	this->insert(this->begin() + pos, value);

	return this;
}


template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_partition(const void* function) {

	LSArray<LSValue*>* array_true = new LSArray<LSValue*>();
	LSArray<LSValue*>* array_false = new LSArray<LSValue*>();
	auto fun = (bool (*)(void*)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_clone(v);
		} else {
			array_false->push_clone(v);
		}
	}
	if (refs == 0) delete this;
	array_true->refs = 1;
	array_false->refs = 1;
	return new LSArray<LSValue*> {array_true, array_false};
}
template <>
inline LSArray<LSValue*>* LSArray<double>::ls_partition(const void* function) {

	LSArray<double>* array_true = new LSArray<double>();
	LSArray<double>* array_false = new LSArray<double>();
	auto fun = (bool (*)(double)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_back(v);
		} else {
			array_false->push_back(v);
		}
	}
	if (refs == 0) delete this;
	array_true->refs = 1;
	array_false->refs = 1;
	return new LSArray<LSValue*> {array_true, array_false};
}
template <>
inline LSArray<LSValue*>* LSArray<int>::ls_partition(const void* function) {

	LSArray<int>* array_true = new LSArray<int>();
	LSArray<int>* array_false = new LSArray<int>();
	auto fun = (bool (*)(int)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_back(v);
		} else {
			array_false->push_back(v);
		}
	}
	if (refs == 0) delete this;
	array_true->refs = 1;
	array_false->refs = 1;
	return new LSArray<LSValue*> {array_true, array_false};
}


template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_map2(LSArray<LSValue*>* array, const void* function) {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		LSValue* v2 = ((LSArray<LSValue*>*) array)->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::ls_map2(LSArray<LSValue*>* array, const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = LSNumber::get(this->operator [] (i));
		LSValue* v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_map2_int(LSArray<int>* array, const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::ls_map2_int(LSArray<int>* array, const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(int, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		int v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline int LSArray<LSValue*>::ls_search(LSValue* needle, int start) {

	for (size_t i = start; i < this->size(); i++) {
		if (needle->operator == ((*this)[i])) {
			if (refs == 0) delete this;
			LSValue::delete_temporary(needle);
			return i;
		}
	}
	if (refs == 0) delete this;
	LSValue::delete_temporary(needle);
	return -1;
}
template <>
inline int LSArray<double>::ls_search(double needle, int start) {

	for (size_t i = start; i < this->size(); i++) {
		if (needle == (*this)[i]) {
			if (refs == 0) delete this;
			return i;
		}
	}
	if (refs == 0) delete this;
	return -1;
}
template <>
inline int LSArray<int>::ls_search(int needle, int start) {

	for (size_t i = start; i < this->size(); i++) {
		if (needle == (*this)[i]) {
			if (refs == 0) delete this;
			return i;
		}
	}
	if (refs == 0) delete this;
	return -1;
}

template <>
inline LSString* LSArray<LSValue*>::ls_join(LSString* glue) {
	if (this->empty()) {
		if (refs == 0) delete this;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	auto it = this->begin();
	LSString* empty = new LSString();
	LSValue* result = (*it)->operator + (empty);
	delete empty;
	for (it++; it != this->end(); it++) {
		LSValue* n1 = glue->operator + (result);
		LSValue* n = (*it)->operator + (n1);
		LSValue::delete_temporary(result);
		LSValue::delete_temporary(n1);
		result = n;
	}
	if (refs == 0) delete this;
	if (glue->refs == 0) delete glue;
	return (LSString*) result;
}
template <>
inline LSString* LSArray<int>::ls_join(LSString* glue) {
	if (this->empty()) {
		if (refs == 0) delete this;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	size_t i = 0;
	std::string result = std::to_string(this->operator[] (i));
	for (i++; i < this->size(); i++) {
		result = result + *glue + std::to_string(this->operator[] (i));
	}
	if (refs == 0) delete this;
	if (glue->refs == 0) delete glue;
	return new LSString(result);
}
template <>
inline LSString* LSArray<double>::ls_join(LSString* glue) {
	if (this->empty()) {
		if (refs == 0) delete this;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	size_t i = 0;
	std::string result = std::to_string(this->operator[] (i));
	for (i++; i < this->size(); i++) {
		result = result + *glue + std::to_string(this->operator[] (i));
	}
	if (refs == 0) delete this;
	if (glue->refs == 0) delete glue;
	return new LSString(result);
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_fill(LSValue* element, int size) {
	this->clear();
	this->reserve(size);
	for (int i = 0; i < size; i++) {
		this->push_move(element);
	}
	LSValue::delete_temporary(element); // only useful if size = 0
	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_fill(T element, int size) {
	this->clear();
	this->resize(size, element);
	return this;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_max() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return LSNull::get();
	}

	LSValue* max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if ((*this)[i]->operator <(max)) {
			max = (*this)[i];
		}
	}
	if (refs == 0) {
		max = max->clone();
		delete this;
	}
	return max;
}
template <typename T>
inline T LSArray<T>::ls_max() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return (T) 0;
	}

	T max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if (max < (*this)[i]) {
			max = (*this)[i];
		}
	}
	if (refs == 0) delete this;
	return max;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_min() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return LSNull::get();
	}

	LSValue* max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if (max->operator <((*this)[i])) {
			max = (*this)[i];
		}
	}
	if (refs == 0) {
		max = max->clone();
		delete this;
	}
	return max;
}
template <typename T>
inline T LSArray<T>::ls_min() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return (T) 0;
	}

	T max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if ((*this)[i] < max) {
			max = (*this)[i];
		}
	}
	if (refs == 0) delete this;
	return max;
}



/*
 * LSValue methods
 */
template <class T>
bool LSArray<T>::isTrue() const {
	return this->size() > 0;
}

template <class T>
LSValue* LSArray<T>::operator ! () const {
	return LSBoolean::get(this->size() == 0);
}

template <class T>
LSValue* LSArray<T>::operator ~ () const {
	LSArray<T>* array = new LSArray<T>();
	array->reserve(this->size());
	for (auto i = this->rbegin(); i != this->rend(); ++i) {
		array->push_clone(*i);
	}
	return array;
}




template <class T>
LSValue* LSArray<T>::operator + (const LSValue* v) const {
	return v->operator + (this);
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSNull* v) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) v);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSNull*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSNull*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSBoolean* boolean) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) boolean);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSBoolean*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSBoolean*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSNumber* v) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) v);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSNumber* number) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone(number->value);
	return new_array;
}
template <>
inline LSValue* LSArray<double>::operator + (const LSNumber* number) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone(number->value);
	return new_array;
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSString* string) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) string);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSString*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSString*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSArray<LSValue*>* array) const {
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->reserve(new_array->size() + array->size());
	for (auto v : *array) {
		new_array->push_clone(v);
	}
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSArray<LSValue*>*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSArray<LSValue*>*) const {
	return LSNull::get();
}
template <class T>
inline LSValue* LSArray<T>::operator + (const LSArray<int>*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator + (const LSArray<int>* array) const {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->insert(new_array->end(), this->begin(), this->end());
	new_array->insert(new_array->end(), array->begin(), array->end());
	return new_array;
}
template <class T>
inline LSValue* LSArray<T>::operator + (const LSArray<double>*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSArray<double>* array) const {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->insert(new_array->end(), this->begin(), this->end());
	new_array->insert(new_array->end(), array->begin(), array->end());
	return new_array;
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSObject* object) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) object);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSObject*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSObject*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSFunction* fun) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) fun);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSFunction*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSFunction*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSClass* clazz) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) clazz);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSClass*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSClass*) const {
	return LSNull::get();
}




template <class T>
LSValue* LSArray<T>::operator += (LSValue* value) {
	return value->operator += (this);
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSNull* null) {
	push_clone((T) null);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSNull*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSNull*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSBoolean* boolean) {
	push_clone((T) boolean);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSBoolean*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSBoolean*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSNumber* num) {
	push_clone((T) num);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSNumber*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSNumber*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSString* string) {
	push_clone((T) string);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSString*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSString*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSArray<LSValue*>* array) {

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
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSObject* object) {
	push_clone((T) object);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSObject*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSObject*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSFunction* fun) {
	push_clone((T) fun);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSFunction*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSFunction*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSClass* clazz) {
	push_clone((T) clazz);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSClass*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSClass*) {
	return LSNull::get();
}


template <class T>
LSValue* LSArray<T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}
template <class T>
LSValue* LSArray<T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}
template <class T>
LSValue* LSArray<T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}
template <class T>
LSValue* LSArray<T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}
template <class T>
LSValue* LSArray<T>::operator / (const LSValue* value) const {
	return value->operator / (value);
}
template <class T>
LSValue* LSArray<T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}
template <class T>
LSValue* LSArray<T>::poww(const LSValue* value) const {
	return value->poww(this);
}
template <class T>
LSValue* LSArray<T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
template <class T>
LSValue* LSArray<T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}
template <class T>
LSValue* LSArray<T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}

template <class T>
bool LSArray<T>::operator == (const LSValue* v) const {
	return v->operator == (this);
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
inline bool LSArray<T>::operator < (const LSValue* v) const {
	return v->operator < (this);
}

template <class T>
inline bool LSArray<T>::operator < (const LSNull*) const {
	return false;
}

template <class T>
inline bool LSArray<T>::operator < (const LSBoolean*) const {
	return false;
}

template <class T>
inline bool LSArray<T>::operator < (const LSNumber*) const {
	return false;
}

template <class T>
inline bool LSArray<T>::operator < (const LSString*) const {
	return false;
}

template <>
inline bool LSArray<LSValue*>::operator < (const LSArray<LSValue*>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end(), [](LSValue* a, LSValue* b) -> bool {
		return a->operator > (b);
	});
}
template <>
inline bool LSArray<int>::operator < (const LSArray<LSValue*>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if ((*it2)->typeID() < 3) return false;
		if (3 < (*it2)->typeID()) return true;
		if (*it1 < ((LSNumber*) *it2)->value) return true;
		if (((LSNumber*) *it2)->value < *it1) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<double>::operator < (const LSArray<LSValue*>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if ((*it2)->typeID() < 3) return false;
		if (3 < (*it2)->typeID()) return true;
		if (*it1 < ((LSNumber*) *it2)->value) return true;
		if (((LSNumber*) *it2)->value < *it1) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<LSValue*>::operator < (const LSArray<int>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if (3 < (*it1)->typeID()) return false;
		if ((*it1)->typeID() < 3) return true;
		if (((LSNumber*) *it1)->value < *it2) return true;
		if (*it2 < ((LSNumber*) *it1)->value) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<int>::operator < (const LSArray<int>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}
template <>
inline bool LSArray<double>::operator < (const LSArray<int>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}
template <>
inline bool LSArray<LSValue*>::operator < (const LSArray<double>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if (3 < (*it1)->typeID()) return false;
		if ((*it1)->typeID() < 3) return true;
		if (((LSNumber*) *it1)->value < *it2) return true;
		if (*it2 < ((LSNumber*) *it1)->value) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<int>::operator < (const LSArray<double>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}
template <>
inline bool LSArray<double>::operator < (const LSArray<double>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}


template <class T>
inline bool LSArray<T>::operator < (const LSObject*) const {
	return true;
}

template <class T>
inline bool LSArray<T>::operator < (const LSFunction*) const {
	return true;
}

template <class T>
inline bool LSArray<T>::operator < (const LSClass*) const {
	return true;
}


template <class T>
inline bool LSArray<T>::in(const LSValue* key) const {
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
inline int LSArray<int>::atv(const int i) {
	return this->operator[] (i);
}

template <>
inline int* LSArray<int>::atLv(int i) {
	return &this->operator[] (i);
}

template <typename T>
inline LSValue* LSArray<T>::range(int start, int end) const {

	LSArray<T>* range = new LSArray<T>();

	size_t start_i = std::max<size_t>(0, start);
	size_t end_i = std::min<size_t>(this->size(), end);

	for (size_t i = start_i; i < end_i; ++i) {
		range->push_clone(this->operator [] (i));
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
	new_array->reserve(this->size());

	for (auto i = this->begin(); i != this->end(); i++) {
		new_array->push_clone(*i);
	}
	return new_array;
}

template <>
inline std::ostream& LSArray<LSValue*>::print(std::ostream& os) const {
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

template <class T>
LSValue* LSArray<T>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue*) ((std::vector<T>*) this)->at((int) n->value)->clone();
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <>
inline LSValue* LSArray<int>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::vector<int>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <>
inline LSValue* LSArray<double>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::vector<double>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <class T>
LSValue* LSArray<T>::attr(const LSValue* key) const {

	if (*((LSString*) key) == "size") {
		return LSNumber::get(this->size());
	}
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}


template <class T>
LSValue** LSArray<T>::atL(const LSValue* key) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			LSValue** v = (LSValue**) &(((std::vector<T>*)this)->at((int) n->value));
			return v;
		} catch (std::exception& e) {
			return nullptr;
		}
	}
	return nullptr;
}

template <class T>
LSValue** LSArray<T>::attrL(const LSValue*) {
	return nullptr;
}

template <class T>
LSValue* LSArray<T>::abso() const {
	return LSNumber::get(this->size());
}

} // end of namespace ls

#endif

