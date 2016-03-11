#include "ArraySTD.hpp"
#include "../value/LSArray.hpp"
#include "../value/LSNumber.hpp"

ArraySTD::ArraySTD() : Module("Array") {}

void ArraySTD::include() {

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

	method("filter", Type::ARRAY, {Type::ARRAY, map_fun_type},(void*)&array_filter);
	method("contains",Type::BOOLEAN_P, {Type::ARRAY, Type::POINTER}, (void*)&array_contains);
	method("isEmpty",Type::BOOLEAN_P, {Type::ARRAY}, (void*)&array_isEmpty);
	method("partition",Type::ARRAY, {Type::ARRAY, map_fun_type}, (void*)&array_partition);
	method("first", Type::POINTER, {Type::ARRAY},(void*)&array_first);
	method("last", Type::POINTER, {Type::ARRAY},(void*)&array_last);
	method("foldLeft", Type::POINTER, {Type::ARRAY, map2_fun_type, Type::POINTER}, (void*)&array_foldLeft);
	method("foldRight", Type::POINTER, {Type::ARRAY, map2_fun_type, Type::POINTER}, (void*)&array_foldRight);

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

}

LSArray* array_clear(const LSArray* array) {

}

LSValue* array_fill(const LSArray* array, const LSValue* value, const LSNumber* size) {

}

LSArray* array_filter(const LSArray* array, const LSFunction* function) {
	LSArray* new_array = new LSArray();
	auto fun = (void* (*)(void*))function->function;
	if (array->associative) {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) new_array->pushKey(v.first, v.second);

	} else {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) new_array->push(v.second);
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

LSValue* array_insert(const LSArray* array, const LSValue* element, const LSValue* index) {

}

LSValue* array_isEmpty(const LSArray* array) {
	return new LSBoolean(array->values.size() == 0);
}

LSValue* array_join(const LSArray* array, const LSString* glue) {

}

LSValue* array_keySort(const LSArray* array, const LSNumber* order) {

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
		new_array->push((LSValue*) fun(v.second));
	}
	return new_array;
}

LSArray* array_map2(const LSArray* array, const LSArray* array2, const LSFunction* function) {
	LSArray* new_array = new LSArray();
	auto fun = (void* (*)(void*, void*))function->function;
	for (auto v : array->values) {
		LSValue* v2 = ((LSArray*) array2)->values[v.first];
		new_array->push((LSValue*) fun(v.second, v2));
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
			if (((LSValue *)fun(v.second))->isTrue()) array_true->pushKey(v.first, v.second);
			else array_false->pushKey(v.first, v.second);

	} else {
		for (auto v : array->values)
			if (((LSValue *)fun(v.second))->isTrue()) array_true->push(v.second);
			else array_false->push(v.second);
	}
	new_array->push(array_true);
	new_array->push(array_false);
	return new_array;
}

LSValue* array_pop(const LSArray* array) {

}

LSValue* array_push(const LSArray* array, LSValue* value) {

}

LSValue* array_pushAll(const LSArray* array, const LSArray* elements) {

}

LSValue* array_remove(const LSArray* array, const LSValue* index) {

}

LSValue* array_removeElement(const LSArray* array, const LSValue* element) {

}

LSValue* array_removeKey(const LSArray* array, const LSValue* index) {

}

LSValue* array_reverse(const LSArray* array) {

}

LSNumber* array_search(const LSArray* array, const LSValue* value, const LSValue* start) {

}

LSValue* array_shift(const LSArray* array) {

}

LSArray* array_shuffle(const LSArray* array) {

}

LSNumber* array_size(const LSArray* array) {
	return LSNumber::get(array->values.size());
}

LSArray* array_sort(const LSArray* array, const LSNumber* order) {

}

LSValue* array_subArray(const LSArray* array, const LSNumber* start, const LSNumber* end) {

}

LSValue* array_sum(const LSArray* array) {
	double sum = 0;
	for (auto v : array->values) {
		sum += ((LSNumber*) v.second)->value;
	}
	return LSNumber::get(sum);
}

LSValue* array_unshift(const LSArray* array, const LSValue* value) {

}
