#ifndef VM_STANDARD_ARRAYSTD_HPP_
#define VM_STANDARD_ARRAYSTD_HPP_

#include "../value/LSArray.hpp"
#include "../value/LSFunction.hpp"
#include "../Module.hpp"

class ArraySTD : public Module {
public:
	ArraySTD();

	void include();
};

LSValue* array_average(const LSArray* array);
LSArray* array_concat(const LSArray* array1, const LSArray* array2);
LSArray* array_clear(const LSArray* array);
LSValue* array_fill(const LSArray* array, const LSValue* value, const LSNumber* size);
LSArray* array_filter(const LSArray* array, const LSFunction* function);
LSValue* array_first(const LSArray* array);
LSArray* array_flatten(const LSArray* array, const LSNumber* depth);
LSValue* array_foldLeft(const LSArray* array, const LSFunction* function, LSValue* v0);
LSValue* array_foldRight(const LSArray* array, const LSFunction* function, LSValue* v0);
LSValue* array_iter(const LSArray* array, const LSFunction* function);
LSValue* array_contains(const LSArray* array, const LSValue* value);
LSValue* array_insert(const LSArray* array, const LSValue* element, const LSValue* index);
LSValue* array_isEmpty(const LSArray* array);
LSValue* array_join(const LSArray* array, const LSString* glue);
LSValue* array_keySort(const LSArray* array, const LSNumber* order);
LSValue* array_last(const LSArray* array);
LSArray* array_map(const LSArray* array, const LSFunction* fun);
LSArray* array_map2(const LSArray* array, const LSArray* array2, const LSFunction* fun);
LSValue* array_max(const LSArray* array);
LSValue* array_min(const LSArray* array);
LSValue* array_partition(const LSArray* array, const LSFunction* callback);
LSValue* array_pop(const LSArray* array);
LSValue* array_push(const LSArray* array, LSValue* value);
LSValue* array_pushAll(const LSArray* array, const LSArray* elements);
LSValue* array_remove(const LSArray* array, const LSValue* index);
LSValue* array_removeElement(const LSArray* array, const LSValue* element);
LSValue* array_removeKey(const LSArray* array, const LSValue* index);
LSValue* array_reverse(const LSArray* array);
LSNumber* array_search(const LSArray* array, const LSValue* value, const LSValue* start);
LSValue* array_shift(const LSArray* array);
LSArray* array_shuffle(const LSArray* array);
LSValue* array_shift(const LSArray* array);
LSNumber* array_size(const LSArray* array);
LSArray* array_sort(const LSArray* array, const LSNumber* order);
LSValue* array_subArray(const LSArray* array, const LSNumber* start, const LSNumber* end);
LSValue* array_sum(const LSArray* array);
LSValue* array_unshift(const LSArray* array, const LSValue* value);


#endif
