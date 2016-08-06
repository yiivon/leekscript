#ifndef VM_STANDARD_ARRAYSTD_HPP_
#define VM_STANDARD_ARRAYSTD_HPP_

#include "../value/LSFunction.hpp"
#include "../Module.hpp"

namespace ls {

class ArraySTD : public Module {
public:
	ArraySTD();
};

LSArray<LSValue*>* array_concat(const LSArray<LSValue*>* array1, const LSArray<LSValue*>* array2);
LSArray<LSValue*>* array_clear(LSArray<LSValue*>* array);
LSArray<LSValue*>* array_filter(const LSArray<LSValue*>* array, const void* function);
LSValue* array_first(const LSArray<LSValue*>* array);
LSArray<LSValue*>* array_flatten(const LSArray<LSValue*>* array, const LSNumber* depth);
LSValue* array_foldLeft(const LSArray<LSValue*>* array, const LSFunction* function, const LSValue* v0);
LSValue* array_foldRight(const LSArray<LSValue*>* array, const LSFunction* function, const LSValue* v0);
LSValue* array_iter(LSArray<LSValue*>* array, const LSFunction* function);
LSValue* array_contains(const LSArray<LSValue*>* array, const LSValue* value);
LSValue* array_contains_int(const LSArray<int>* array, int value);
LSValue* array_insert(LSArray<LSValue*>* array, const LSValue* element, const LSValue* index);
LSValue* array_isEmpty(const LSArray<LSValue*>* array);
LSValue* array_keySort(const LSArray<LSValue*>* array, const LSNumber* order);
LSValue* array_last(const LSArray<LSValue*>* array);
LSValue* array_max(const LSArray<LSValue*>* array);
LSValue* array_min(const LSArray<LSValue*>* array);
LSValue* array_push(LSArray<LSValue*>* array, LSValue* value);
LSValue* array_pushAll(LSArray<LSValue*>* array, const LSArray<LSValue*>* elements);
LSValue* array_remove(LSArray<LSValue*>* array, const LSValue* index);
LSValue* array_removeElement(LSArray<LSValue*>* array, const LSValue* element);
LSValue* array_removeKey(LSArray<LSValue*>* array, const LSValue* index);
LSValue* array_reverse(const LSArray<LSValue*>* array);
LSValue* array_shift(const LSArray<LSValue*>* array);
LSArray<LSValue*>* array_shuffle(const LSArray<LSValue*>* array);
LSValue* array_shift(const LSArray<LSValue*>* array);
LSArray<LSValue*>* array_sort(const LSArray<LSValue*>* array, const LSNumber* order);
LSValue* array_sum(const LSArray<LSValue*>* array);
LSValue* array_unshift(const LSArray<LSValue*>* array, const LSValue* value);
LSArray<LSValue*>* array_chunk_1_ptr(const LSArray<LSValue*>* array);
LSArray<LSValue*>* array_chunk_1_int(const LSArray<int>* array);
LSArray<LSValue*>* array_chunk_1_float(const LSArray<double>* array);

}

#endif
