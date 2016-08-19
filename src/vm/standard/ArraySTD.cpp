#include <algorithm>
#include "ArraySTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

using namespace std;

namespace ls {

LSValue* array_first(const LSArray<LSValue*>* array);
LSArray<LSValue*>* array_flatten(const LSArray<LSValue*>* array, const LSNumber* depth);
LSValue* array_foldLeft(const LSArray<LSValue*>* array, const LSFunction* function, const LSValue* v0);
LSValue* array_foldRight(const LSArray<LSValue*>* array, const LSFunction* function, const LSValue* v0);
LSValue* array_contains(const LSArray<LSValue*>* array, const LSValue* value);
LSValue* array_contains_int(const LSArray<int>* array, int value);
LSValue* array_insert(LSArray<LSValue*>* array, const LSValue* element, const LSValue* index);
LSValue* array_keySort(const LSArray<LSValue*>* array, const LSNumber* order);
LSValue* array_last(const LSArray<LSValue*>* array);
LSValue* array_push(LSArray<LSValue*>* array, LSValue* value);
LSValue* array_pushAll(LSArray<LSValue*>* array, const LSArray<LSValue*>* elements);
LSValue* array_shift(const LSArray<LSValue*>* array);
LSValue* array_shift(const LSArray<LSValue*>* array);
LSArray<LSValue*>* array_sort(const LSArray<LSValue*>* array, const LSNumber* order);
LSValue* array_sum(const LSArray<LSValue*>* array);
LSValue* array_unshift(const LSArray<LSValue*>* array, const LSValue* value);
LSArray<LSValue*>* array_chunk_1_ptr(LSArray<LSValue*>* array);
LSArray<LSValue*>* array_chunk_1_int(LSArray<int>* array);
LSArray<LSValue*>* array_chunk_1_float(LSArray<double>* array);
LSValue* array_sub(LSArray<LSValue*>* array, int begin, int end);

ArraySTD::ArraySTD() : Module("Array") {

	method("average", {
		{Type::ARRAY, Type::FLOAT, {}, (void*) &LSArray<LSValue*>::ls_average},
		{Type::FLOAT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<double>::ls_average},
		{Type::INT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<int>::ls_average}
	});

	method("max", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_max},
		{Type::FLOAT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<double>::ls_max},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_max}
	});

	method("min", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_min},
		{Type::FLOAT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<double>::ls_min},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_min}
	});

	method("size", {
		{Type::ARRAY, Type::INTEGER, {}, (void*) &LSArray<LSValue*>::ls_size},
		{Type::FLOAT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<double>::ls_size},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_size}
	});

	method("sum", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_sum},
		{Type::FLOAT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<double>::ls_sum},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_sum}
	});

	Type map_int_fun_type = Type::FUNCTION;
	map_int_fun_type.setArgumentType(0, Type::INTEGER);
	map_int_fun_type.setReturnType(Type::POINTER);

	Type map_fun_type = Type::FUNCTION;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);

	method("map", {
		{Type::ARRAY, Type::ARRAY, {map_fun_type}, (void*) &LSArray<LSValue*>::ls_map},
		{Type::INT_ARRAY, Type::ARRAY, {map_int_fun_type}, (void*) &LSArray<int>::ls_map},
	});

	method("chunk", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) array_chunk_1_ptr},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) array_chunk_1_float},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) array_chunk_1_int},

		{Type::ARRAY, Type::ARRAY, {Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_chunk},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {Type::INTEGER}, (void*) &LSArray<double>::ls_chunk},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_chunk},
    });

	method("unique", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_unique},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_unique},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_unique},
	});

	method("sort", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_sort},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_sort},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_sort},
	});

	Type map2_fun_type = Type::FUNCTION;
	map2_fun_type.setArgumentType(0, Type::POINTER);
	map2_fun_type.setArgumentType(1, Type::POINTER);
	map2_fun_type.setReturnType(Type::POINTER);

	Type map2_fun_type_int = Type::FUNCTION;
	map2_fun_type_int.setArgumentType(0, Type::POINTER);
	map2_fun_type_int.setArgumentType(1, Type::INTEGER);
	map2_fun_type_int.setReturnType(Type::POINTER);

	method("map2", {
		{Type::ARRAY, Type::ARRAY, {Type::ARRAY, map2_fun_type}, (void*) &LSArray<LSValue*>::ls_map2},
		{Type::ARRAY, Type::ARRAY, {Type::INT_ARRAY, map2_fun_type_int}, (void*) &LSArray<LSValue*>::ls_map2_int},
	});

	Type iter_fun_type = Type::FUNCTION;
	iter_fun_type.setArgumentType(0, Type::POINTER);
	iter_fun_type.setReturnType(Type::VOID);
	Type iter_fun_type_int = Type::FUNCTION;
	iter_fun_type_int.setArgumentType(0, Type::INTEGER);
	iter_fun_type_int.setReturnType(Type::VOID);
	Type iter_fun_type_float = Type::FUNCTION;
	iter_fun_type_float.setArgumentType(0, Type::FLOAT);
	iter_fun_type_float.setReturnType(Type::VOID);
	method("iter", {
			   {Type::ARRAY, Type::VOID, {iter_fun_type},(void*) &LSArray<LSValue*>::ls_iter},
			   {Type::FLOAT_ARRAY, Type::VOID, {iter_fun_type_float},(void*) &LSArray<double>::ls_iter},
			   {Type::INT_ARRAY, Type::VOID, {iter_fun_type_int},(void*) &LSArray<int>::ls_iter},
		   });

	Type pred_fun_type = Type::FUNCTION;
	pred_fun_type.setArgumentType(0, Type::POINTER);
	pred_fun_type.setReturnType(Type::BOOLEAN);
	Type pred_fun_type_float = Type::FUNCTION;
	pred_fun_type_float.setArgumentType(0, Type::FLOAT);
	pred_fun_type_float.setReturnType(Type::BOOLEAN);
	Type pred_fun_type_int = Type::FUNCTION;
	pred_fun_type_int.setArgumentType(0, Type::INTEGER);
	pred_fun_type_int.setReturnType(Type::BOOLEAN);
	method("filter", {
		{Type::ARRAY, Type::ARRAY, {pred_fun_type}, (void*) &LSArray<LSValue*>::ls_filter},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {pred_fun_type_float}, (void*) &LSArray<double>::ls_filter},
		{Type::INT_ARRAY, Type::INT_ARRAY, {pred_fun_type_int}, (void*) &LSArray<int>::ls_filter},
	});

	method("contains", {
		{Type::ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_contains},
		{Type::FLOAT_ARRAY, Type::BOOLEAN, {Type::FLOAT}, (void*) &LSArray<double>::ls_contains},
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSArray<int>::ls_contains}
	});

	method("isEmpty", {
		{Type::ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<LSValue*>::ls_empty},
		{Type::FLOAT_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<double>::ls_empty},
		{Type::INT_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<int>::ls_empty},
	});

	method("partition", {
		{Type::ARRAY, Type::ARRAY, {pred_fun_type}, (void*) &LSArray<LSValue*>::ls_partition},
		{Type::FLOAT_ARRAY, Type::ARRAY, {pred_fun_type_float}, (void*) &LSArray<double>::ls_partition},
		{Type::INT_ARRAY, Type::ARRAY, {pred_fun_type_int}, (void*) &LSArray<int>::ls_partition}
	});

	method("first", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_first},
		{Type::FLOAT_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_first},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_first},
	});
	method("last", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_last},
		{Type::FLOAT_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_last},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_last},
	});

	Type fold_fun_type = Type::FUNCTION;
	fold_fun_type.setArgumentType(0, Type::POINTER);
	fold_fun_type.setArgumentType(1, Type::POINTER);
	fold_fun_type.setReturnType(Type::POINTER);
	Type fold_fun_type_float = Type::FUNCTION;
	fold_fun_type_float.setArgumentType(0, Type::POINTER);
	fold_fun_type_float.setArgumentType(1, Type::FLOAT);
	fold_fun_type_float.setReturnType(Type::POINTER);
	Type fold_fun_type_int = Type::FUNCTION;
	fold_fun_type_int.setArgumentType(0, Type::POINTER);
	fold_fun_type_int.setArgumentType(1, Type::INTEGER);
	fold_fun_type_int.setReturnType(Type::POINTER);
	method("foldLeft", {
		{Type::ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldLeft},
		{Type::FLOAT_ARRAY, Type::POINTER, {fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldLeft},
		{Type::INT_ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<int>::ls_foldLeft},
	});
	method("foldRight", {
		{Type::ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldRight},
		{Type::FLOAT_ARRAY, Type::POINTER, {fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldRight},
		{Type::INT_ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<int>::ls_foldRight},
	});

	method("search", {
		{Type::ARRAY, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_search},
		{Type::FLOAT_ARRAY, Type::INTEGER, {Type::FLOAT, Type::INTEGER}, (void*) &LSArray<double>::ls_search},
		{Type::INT_ARRAY, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_search}
	});

	method("pop", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_pop},
		{Type::FLOAT_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_pop},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_pop}
	});

	method("push", {
		{Type::ARRAY, Type::ARRAY, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_push},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {Type::FLOAT}, (void*) &LSArray<double>::ls_push},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_push},
	});

	method("pushAll", {
		{Type::ARRAY, Type::ARRAY, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_push_all},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_push_all}
	});

	method("join", {
		{Type::ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<LSValue*>::ls_join},
		{Type::FLOAT_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<double>::ls_join},
		{Type::INT_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<int>::ls_join}
	});

	method("clear", {
			   {Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_clear},
			   {Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_clear},
			   {Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_clear},
		   });

	method("fill", {
		{Type::ARRAY, Type::ARRAY, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_fill},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {Type::FLOAT, Type::INTEGER}, (void*) &LSArray<double>::ls_fill},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_fill}
	});

	method("insert", {
		{Type::ARRAY, Type::ARRAY, {Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::insert_v},
		{Type::ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::POINTER}, (void*) &LSArray<int>::insert_v}
	});

	method("remove", {
		{Type::ARRAY, Type::POINTER, {Type::INTEGER}, (void*)&LSArray<LSValue*>::ls_remove},
		{Type::FLOAT_ARRAY, Type::FLOAT, {Type::INTEGER}, (void*)&LSArray<double>::ls_remove},
		{Type::INT_ARRAY, Type::INTEGER, {Type::INTEGER}, (void*)&LSArray<int>::ls_remove},
	});

	method("removeElement", {
		{Type::ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*)&LSArray<LSValue*>::ls_remove_element},
		{Type::FLOAT_ARRAY, Type::BOOLEAN, {Type::FLOAT}, (void*)&LSArray<double>::ls_remove_element},
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*)&LSArray<int>::ls_remove_element},
	});

	method("reverse", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_reverse},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_reverse},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_reverse},
	});

	method("shuffle", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_shuffle},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_shuffle},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_shuffle},
	});


	/*
	 * Static methods
	 */
	static_method("average", {
		{Type::FLOAT, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_average},
		{Type::FLOAT, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_average},
		{Type::FLOAT, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_average}
	});

	static_method("size", {
		{Type::INTEGER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_size},
		{Type::INTEGER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_size},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_size}
	});

	static_method("sum", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_sum},
		{Type::POINTER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_sum},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_sum}
	});

	static_method("map", {
		{Type::ARRAY, {Type::ARRAY, map_fun_type}, (void*) &LSArray<LSValue*>::ls_map},
		{Type::ARRAY, {Type::INT_ARRAY, map_int_fun_type}, (void*) &LSArray<int>::ls_map},
	});

	static_method("map2", {
		{Type::ARRAY, {Type::ARRAY, Type::ARRAY, map2_fun_type}, (void*) &LSArray<LSValue*>::ls_map2},
		{Type::ARRAY, {Type::ARRAY, Type::INT_ARRAY, map2_fun_type_int}, (void*) &LSArray<LSValue*>::ls_map2_int},
	});

	static_method("iter", {
					  {Type::VOID, {Type::ARRAY, iter_fun_type},(void*) &LSArray<LSValue*>::ls_iter},
					  {Type::VOID, {Type::FLOAT_ARRAY, iter_fun_type_float},(void*) &LSArray<double>::ls_iter},
					  {Type::VOID, {Type::INT_ARRAY, iter_fun_type_int},(void*) &LSArray<int>::ls_iter},
				  });

	static_method("filter", {
		{Type::ARRAY, {Type::ARRAY, pred_fun_type}, (void*) &LSArray<LSValue*>::ls_filter},
		{Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY, pred_fun_type_float}, (void*) &LSArray<double>::ls_filter},
		{Type::INT_ARRAY, {Type::INT_ARRAY, pred_fun_type_int}, (void*) &LSArray<int>::ls_filter},
	});

	static_method("contains", {
		{Type::BOOLEAN, {Type::PTR_ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_contains},
		{Type::BOOLEAN, {Type::FLOAT_ARRAY, Type::FLOAT}, (void*) &LSArray<double>::ls_contains},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_contains}
	});
	static_method("isEmpty", {
		{Type::BOOLEAN, {Type::ARRAY}, (void*)&LSArray<LSValue*>::ls_empty},
		{Type::BOOLEAN, {Type::FLOAT_ARRAY}, (void*)&LSArray<double>::ls_empty},
		{Type::BOOLEAN, {Type::INT_ARRAY}, (void*)&LSArray<int>::ls_empty}
	});

	static_method("partition", {
		{Type::ARRAY, {Type::ARRAY, pred_fun_type}, (void*) &LSArray<LSValue*>::ls_partition},
		{Type::ARRAY, {Type::FLOAT_ARRAY, pred_fun_type_float}, (void*) &LSArray<double>::ls_partition},
		{Type::ARRAY, {Type::INT_ARRAY, pred_fun_type_int}, (void*) &LSArray<int>::ls_partition}
	});

	static_method("first", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_first},
		{Type::POINTER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_first},
		{Type::POINTER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_first},
	});
	static_method("last", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_last},
		{Type::POINTER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_last},
		{Type::POINTER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_last},
	});

	static_method("foldLeft", {
		{Type::POINTER, {Type::ARRAY, fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldLeft},
		{Type::POINTER, {Type::FLOAT_ARRAY, fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldLeft},
		{Type::POINTER, {Type::INT_ARRAY, fold_fun_type_int, Type::POINTER}, (void*) &LSArray<int>::ls_foldLeft}
	});
	static_method("foldRight", {
		{Type::POINTER, {Type::ARRAY, fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldRight},
		{Type::POINTER, {Type::FLOAT_ARRAY, fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldRight},
		{Type::POINTER, {Type::INT_ARRAY, fold_fun_type_int, Type::POINTER}, (void*) &LSArray<int>::ls_foldRight}
	});

	static_method("shuffle", {
					  {Type::ARRAY, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_shuffle},
					  {Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_shuffle},
					  {Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_shuffle},
				  });
	static_method("reverse", {
					  {Type::ARRAY, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_reverse},
					  {Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_reverse},
					  {Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_reverse},
				  });

	static_method("search", {
		{Type::INTEGER, {Type::ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_search},
		{Type::INTEGER, {Type::FLOAT_ARRAY, Type::FLOAT, Type::INTEGER}, (void*) &LSArray<double>::ls_search},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_search}
	});

	static_method("subArray", {
		{Type::ARRAY, {Type::ARRAY, Type::INTEGER, Type::INTEGER}, (void* )&array_sub}
	});

	static_method("pop", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_pop},
		{Type::INTEGER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_pop},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_pop}
	});

	static_method("push", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_push},
		{Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY, Type::FLOAT}, (void*) &LSArray<double>::ls_push},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_push}
	});

	static_method("pushAll", {
					  {Type::ARRAY, {Type::ARRAY, Type::ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all},
					  {Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY, Type::FLOAT_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all},
					  {Type::INT_ARRAY, {Type::INT_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all},
				  });

	static_method("join", {
		{Type::STRING, {Type::ARRAY, Type::STRING}, (void*) &LSArray<LSValue*>::ls_join},
		{Type::STRING, {Type::FLOAT_ARRAY, Type::STRING}, (void*) &LSArray<double>::ls_join},
		{Type::STRING, {Type::INT_ARRAY, Type::STRING}, (void*) &LSArray<int>::ls_join}
	});

	static_method("clear", {
		{Type::ARRAY, {Type::ARRAY}, (void*)&LSArray<LSValue*>::ls_clear},
		{Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY}, (void*)&LSArray<double>::ls_clear},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*)&LSArray<int>::ls_clear},
	});

	static_method("fill", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_fill},
		{Type::FLOAT_ARRAY, {Type::FLOAT_ARRAY, Type::FLOAT, Type::INTEGER}, (void*) &LSArray<double>::ls_fill},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_fill}
	});

	static_method("insert", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::insert_v},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::POINTER}, (void*) &LSArray<int>::insert_v}
	});

	static_method("remove", {
		{Type::POINTER, {Type::ARRAY, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_remove},
		{Type::FLOAT, {Type::FLOAT_ARRAY, Type::INTEGER}, (void*) &LSArray<double>::ls_remove},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_remove}
	});

	static_method("removeElement", {
		{Type::BOOLEAN, {Type::ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_remove_element},
		{Type::BOOLEAN, {Type::FLOAT_ARRAY, Type::FLOAT}, (void*) &LSArray<double>::ls_remove_element},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_remove_element}
	});
}

LSValue* array_fill(LSArray<LSValue*>* array, const LSValue* value, const LSNumber* size) {
	array->clear();
	for (int i = 0; i < (int) size->value; i++) {
		array->push_clone((LSValue*) value);
	}
	return array;
}

LSArray<LSValue*>* array_flatten(const LSArray<LSValue*>*, const LSNumber*) {
	// TODO
	return new LSArray<LSValue*>();
}

LSValue* array_insert(LSArray<LSValue*>* array, const LSValue* element, const LSValue* index) {
	return array->insert_v((LSValue*) element, index);
}

LSValue* array_shift(const LSArray<LSValue*>*) {
	// TODO
	return new LSArray<LSValue*>();
}

LSValue* array_sum(const LSArray<LSValue*>*) {
//	return LSNumber::get(array->sum());
	return LSNumber::get(0);
}

LSValue* array_unshift(const LSArray<LSValue*>*, const LSValue*) {
	// TODO
	return new LSArray<LSValue*>();
}

LSArray<LSValue*>* array_chunk_1_ptr(LSArray<LSValue*>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* array_chunk_1_int(LSArray<int>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* array_chunk_1_float(LSArray<double>* array) {
	return array->ls_chunk(1);
}

LSValue* array_sub(LSArray<LSValue*>* array, int begin, int end) {
	LSValue* r = array->range(begin, end);
	if (array->refs == 0) delete array;
	return r;
}

}
