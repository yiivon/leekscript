#include <algorithm>
#include "ArraySTD.hpp"
#include "../value/LSArray.hpp"
#include "../value/LSNumber.hpp"

using namespace std;

ArraySTD::ArraySTD() : Module("Array") {

	method("average", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_average);

	method("max", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_max);
	method("min", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_min);

	method("size", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_size);

	method("sum", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_sum);

	Type map_fun_type = Type::FUNCTION_P;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);
	method("map", Type::ARRAY, {Type::ARRAY, map_fun_type}, (void*) &array_map);

	Type map2_fun_type = Type::FUNCTION_P;
	map2_fun_type.setArgumentType(0, Type::POINTER);
	map2_fun_type.setArgumentType(1, Type::POINTER);
	map2_fun_type.setReturnType(Type::POINTER);
	method("map2", Type::ARRAY, {Type::ARRAY, Type::ARRAY, map2_fun_type}, (void*) &array_map2);

	Type iter_fun_type = Type::FUNCTION_P;
	iter_fun_type.setArgumentType(0, Type::POINTER);
	iter_fun_type.setReturnType(Type::POINTER);
	method("iter", Type::POINTER, {Type::ARRAY, iter_fun_type},(void*)&array_iter);

	Type filter_fun_type = Type::FUNCTION_P;
	filter_fun_type.setArgumentType(0, Type::POINTER);
	filter_fun_type.setReturnType(Type::POINTER);
	method("filter", Type::ARRAY, {Type::ARRAY, filter_fun_type},(void*)&array_filter);
	method("contains",Type::BOOLEAN_P, {Type::ARRAY, Type::POINTER}, (void*)&array_contains);
	method("isEmpty",Type::BOOLEAN_P, {Type::ARRAY}, (void*)&array_isEmpty);
	method("partition",Type::ARRAY, {Type::ARRAY, map_fun_type}, (void*)&array_partition);
	method("first", Type::POINTER, {Type::ARRAY}, (void*)&array_first);
	method("last", Type::POINTER, {Type::ARRAY}, (void*)&array_last);
	method("foldLeft", Type::POINTER, {Type::ARRAY, map2_fun_type, Type::POINTER}, (void*)&array_foldLeft);
	method("foldRight", Type::POINTER, {Type::ARRAY, map2_fun_type, Type::POINTER}, (void*)&array_foldRight);
	method("reverse", Type::ARRAY, {Type::ARRAY}, (void*)&array_reverse);
	method("shuffle", Type::ARRAY, {Type::ARRAY}, (void*)&array_shuffle);
	method("search", Type::POINTER, {Type::ARRAY, Type::POINTER, Type::POINTER}, (void*)&array_search);
	method("subArray", Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::POINTER}, (void*)&array_subArray);
	method("pop", Type::POINTER, {Type::ARRAY}, (void*)&array_pop);
	method("push", Type::ARRAY, {Type::ARRAY}, (void*)&array_push);
	method("pushAll", Type::ARRAY, {Type::ARRAY, Type::ARRAY}, (void*)&array_pushAll);
	method("concat", Type::ARRAY, {Type::ARRAY, Type::ARRAY}, (void*)&array_concat);
	method("join", Type::STRING, {Type::ARRAY, Type::STRING}, (void*)&array_join);
	method("clear", Type::ARRAY, {Type::ARRAY}, (void*)&array_clear);
	method("fill", Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::INTEGER_P}, (void*)&array_fill);
	method("insert", Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::POINTER}, (void*)&array_insert);
	method("remove", Type::POINTER, {Type::ARRAY, Type::POINTER}, (void*)&array_remove);
	method("removeKey", Type::POINTER, {Type::ARRAY, Type::POINTER}, (void*)&array_removeKey);
	method("removeElement", Type::ARRAY, {Type::ARRAY, Type::POINTER}, (void*)&array_removeElement);
}

LSValue* array_average(const LSArray* array) {
	if (array->values.size() == 0) {
		return LSNumber::get(0);
	}
	double avg = 0;
	for (auto v : array->values) {
		avg += ((LSNumber*) v.second)->value;
	}
	return LSNumber::get(avg / array->values.size());
}

LSArray* array_concat(const LSArray* array1, const LSArray* array2) {
	return (LSArray*) array1->operator +(array2);
}

LSArray* array_clear(LSArray* array) {
	array->clear();
	return array;
}

LSValue* array_fill(LSArray* array, const LSValue* value, const LSNumber* size) {
	array->clear();
	for (int i = 0; i < (int) size->value; i++) {
		array->pushClone((LSValue*) value);
	}
	return array;
}

LSArray* array_filter(const LSArray* array, const LSFunction* function) {
	LSArray* new_array = new LSArray();
	auto fun = (void* (*)(void*))function->function;
	if (array->associative) {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) new_array->pushKeyClone(v.first, v.second);

	} else {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) new_array->pushClone(v.second);
	}
	return new_array;
}

LSValue* array_first(const LSArray* array) {
	auto first = array->values.begin();
	if (first == array->values.end())
		return LSNull::null_var;
	return first->second->clone();
}

LSArray* array_flatten(const LSArray* array, const LSNumber* depth) {
	return new LSArray();
}

LSValue* array_foldLeft(const LSArray* array, const LSFunction* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*,LSValue*))function->function;
	LSValue* result = v0;
	for (auto v : array->values)
		result = fun(result, v.second);
	return result;
}

LSValue* array_foldRight(const LSArray* array, const LSFunction* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*,LSValue*))function->function;
	LSValue* result = v0;
	for (auto it = array->values.rbegin(); it != array->values.rend(); it++)
		result = fun(it->second, result);
	return result;
}

LSValue* array_iter(const LSArray* array, const LSFunction* function) {
	auto fun = (void* (*)(void*))function->function;
	for (auto v : array->values)
		fun(v.second);
	return LSNull::null_var;
}

LSValue* array_contains(const LSArray* array, const LSValue* value) {
	for (auto v : array->values) {
		if (value->operator ==(v.second))
			return LSBoolean::true_val;
	}
	return LSBoolean::false_val;
}

LSValue* array_insert(LSArray* array, const LSValue* element, const LSValue* index) {
	if (not array->associative and index->isInteger() and ((LSNumber*)index)->value <= array ->index and ((LSNumber*)index)->value >= 0) {
		if ((int)((LSNumber*)index)->value == array ->index) {
			array->pushClone((LSValue*) element);
		} else {
			// TODO should move all elements after index to the right ? or replace the element
			array->values[(LSValue*) index] = element->clone();
		}
	} else {
		array->pushKeyClone((LSValue*) index, (LSValue*) element);
	}
	return array;
}

LSValue* array_isEmpty(const LSArray* array) {
	return new LSBoolean(array->values.empty());
}

LSValue* array_join(const LSArray* array, const LSString* glue) {
	if (array->values.empty())
		return new LSString();
	auto it = array->values.begin();
	LSValue* result = it->second->operator +(new LSString());
	for (it++; it != array->values.end(); it++) {
		result = it->second->operator +(glue->operator +(result));
	}
	return result;
}

LSValue* array_keySort(const LSArray* array, const LSNumber* order) {
	return new LSArray();
}

LSValue* array_last(const LSArray* array) {
	auto last = array->values.rbegin();
	if (last == array->values.rend())
		return LSNull::null_var;
	return last->second->clone();
}

LSArray* array_map(const LSArray* array, const LSFunction* function) {
	LSArray* new_array = new LSArray();
	auto fun = (void* (*)(void*))function->function;
	for (auto v : array->values) {
		new_array->pushClone((LSValue*) fun(v.second));
	}
	return new_array;
}

LSArray* array_map2(const LSArray* array, const LSArray* array2, const LSFunction* function) {
	LSArray* new_array = new LSArray();
	auto fun = (void* (*)(void*, void*))function->function;
	for (auto v : array->values) {
		LSValue* v2 = ((LSArray*) array2)->values[v.first];
		new_array->pushClone((LSValue*) fun(v.second, v2));
	}
	return new_array;
}

LSValue* array_max(const LSArray* array) {
	if (array->values.size() == 0) {
		return LSNumber::get(0);
	}
	auto it = array->values.begin();
	double max = ((LSNumber*) it->second)->value;
	it++;
	while (it != array->values.end()) {
		double val = ((LSNumber*) it->second)->value;
		if (val > max) {
			max = val;
		}
		it++;
	}
	return LSNumber::get(max);
}

LSValue* array_min(const LSArray* array) {
	if (array->values.size() == 0) {
		return LSNumber::get(0);
	}
	auto it = array->values.begin();
	double min = ((LSNumber*) it->second)->value;
	it++;
	while (it != array->values.end()) {
		double val = ((LSNumber*) it->second)->value;
		if (val < min) {
			min = val;
		}
		it++;
	}
	return LSNumber::get(min);
}

LSValue* array_partition(const LSArray* array, const LSFunction* callback) {
	LSArray* new_array = new LSArray();
	LSArray* array_true = new LSArray();
	LSArray* array_false = new LSArray();
	auto fun = (void* (*)(void*))callback->function;
	if (array->associative) {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) array_true->pushKeyClone(v.first, v.second);
			else array_false->pushKeyClone(v.first, v.second);

	} else {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) array_true->pushClone(v.second);
			else array_false->pushClone(v.second);
	}
	new_array->pushClone(array_true);
	new_array->pushClone(array_false);
	return new_array;
}

LSValue* array_pop(LSArray* array) {
	return array->pop();
}

LSValue* array_push(LSArray* array, LSValue* value) {
	array->pushClone(value);
	return array;
}

LSValue* array_pushAll(LSArray* array, const LSArray* elements) {
	if (not (array->associative and elements->associative)) {
		for (auto i = elements->values.begin(); i != elements->values.end(); ++i) {
			array->pushClone(i->second);
		}
	}
	return array;
}

LSValue* array_remove(LSArray* array, const LSValue* index) {
	if (not array->associative and index->isInteger() and ((LSNumber*)index)->value < array ->index and ((LSNumber*)index)->value >= 0) {
		return array->remove((LSNumber*) index);
	} else {
		return array->removeKey((LSValue*) index);
	}
}

LSValue* array_removeElement(LSArray* array, const LSValue* element) {
	for (auto i = array->values.begin(); i != array->values.end(); i++) {
		if (i->second->operator ==(element)){
			if (array->associative) {
				array->removeKey(i->first);
			} else {
				array->remove((LSNumber*) i->first);
			}
			break;
		}
	}
	return array;
}

LSValue* array_removeKey(LSArray* array, const LSValue* index) {
	return array->removeKey((LSValue*) index);
}

LSValue* array_reverse(const LSArray* array) {
	LSArray* new_array = new LSArray();
	for (auto it = array->values.rbegin(); it != array->values.rend(); it++) {
		new_array->pushClone(it->second);
	}
	return new_array;
}

LSValue* array_search(const LSArray* array, const LSValue* value, const LSValue* start) {
	for (auto i = array->values.begin(); i != array->values.end(); i++) {
		if (start->operator < (i->first)) continue; // i < start
		if (value->operator == (i->second))
			return i->first->clone();
	}
	return LSNull::null_var;
}

LSValue* array_shift(const LSArray* array) {
	return new LSArray();
}

LSArray* array_shuffle(const LSArray* array) {
	LSArray* new_array = new LSArray();
	if (array->values.empty())
		return new_array;
	vector<LSValue*> shuffled_values;
	for (auto it = array->values.begin(); it != array->values.end(); it++) {
		shuffled_values.push_back(it->second);
	}
	random_shuffle(shuffled_values.begin(), shuffled_values.end());
	for (auto it = shuffled_values.begin(); it != shuffled_values.end(); it++) {
		new_array->pushClone(*it);
	}
	return new_array;
}

LSNumber* array_size(const LSArray* array) {
	return LSNumber::get(array->values.size());
}

LSArray* array_sort(const LSArray* array, const LSNumber* order) {
	return new LSArray();
}

LSValue* array_subArray(const LSArray* array, const LSNumber* start, const LSNumber* end) {
	return array->range(start,end);
}

LSValue* array_sum(const LSArray* array) {
	double sum = 0;
	for (auto v : array->values) {
		sum += ((LSNumber*) v.second)->value;
	}
	return LSNumber::get(sum);
}

LSValue* array_unshift(const LSArray* array, const LSValue* value) {
	return new LSArray();
}
