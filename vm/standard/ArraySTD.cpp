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

}

LSArray* array_flatten(const LSArray* array, const LSNumber* depth) {

}

LSValue* array_foldLeft(const LSArray* array, const LSFunction* function, LSValue* v0) {

}

LSValue* array_foldRight(const LSArray* array, const LSFunction* function, LSValue* v0) {

}

LSValue* array_iter(const LSArray* array, const LSFunction* function) {

}

LSValue* array_contains(const LSArray* array, const LSValue* value) {

}

LSValue* array_insert(const LSArray* array, const LSValue* element, const LSValue* index) {

}

LSValue* array_isEmpty(const LSArray* array) {

}

LSValue* array_join(const LSArray* array, const LSString* glue) {

}

LSValue* array_keySort(const LSArray* array, const LSNumber* order) {

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
