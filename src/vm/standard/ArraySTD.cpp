#include <algorithm>
#include "ArraySTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"
#include "ValueSTD.hpp"
#include "../TypeMutator.hpp"

namespace ls {

ArraySTD::ArraySTD() : Module("Array") {

	LSArray<LSValue*>::clazz = clazz;
	LSArray<int>::clazz = clazz;
	LSArray<double>::clazz = clazz;

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::CONST_PTR_ARRAY, Type::CONST_ANY, Type::BOOLEAN, (void*) &LSArray<LSValue*>::in, {}, Method::NATIVE},
		{Type::CONST_REAL_ARRAY, Type::CONST_REAL, Type::BOOLEAN, (void*) &LSArray<double>::in, {}, Method::NATIVE},
		{Type::CONST_INT_ARRAY, Type::CONST_INTEGER, Type::BOOLEAN, (void*) &LSArray<int>::in_i, {}, Method::NATIVE}
	});
	operator_("+=", {
		{Type::ARRAY, Type::CONST_ANY, Type::ANY, (void*) &array_add_eq, {new WillStoreMutator()}, false, true},
		{Type::REAL_ARRAY, Type::CONST_REAL, Type::REAL_ARRAY, (void*) &LSArray<double>::add_eq_double, {new WillStoreMutator()}, Method::NATIVE},
		{Type::INT_ARRAY, Type::CONST_INTEGER, Type::INT_ARRAY, (void*) &LSArray<int>::add_eq_int, {new WillStoreMutator()}, Method::NATIVE}
	});

	/* Type tilde_tilde_fun_type = Type::FUNCTION;
	tilde_tilde_fun_type.setArgumentType(0, Type::T);
	tilde_tilde_fun_type.setReturnType(Type::ANY);
	operator_("~~", {
		{Type::T_ARRAY, tilde_tilde_fun_type, Type::PTR_ARRAY, (void*) &LSArray<LSValue*>::ls_map}
	});*/

	/*
	 * Methods
	 */
	method("copy", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY}, (void*) &ValueSTD::copy},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY}, (void*) &ValueSTD::copy},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY}, (void*) &ValueSTD::copy},
	});
	method("average", {
		{Type::REAL, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_average, Method::NATIVE},
		{Type::REAL, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_average, Method::NATIVE},
		{Type::REAL, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_average, Method::NATIVE},
	});

	method("chunk", {
		{Type::PTR_ARRAY_ARRAY, {Type::CONST_ARRAY}, (void*) ArraySTD::chunk_1_ptr, Method::NATIVE},
		{Type::REAL_ARRAY_ARRAY, {Type::CONST_REAL_ARRAY}, (void*) ArraySTD::chunk_1_float, Method::NATIVE},
		{Type::INT_ARRAY_ARRAY, {Type::CONST_INT_ARRAY}, (void*) ArraySTD::chunk_1_int, Method::NATIVE},
		{Type::PTR_ARRAY_ARRAY, {Type::CONST_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<LSValue*>::ls_chunk, Method::NATIVE},
		{Type::REAL_ARRAY_ARRAY, {Type::CONST_REAL_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<double>::ls_chunk, Method::NATIVE},
		{Type::INT_ARRAY_ARRAY, {Type::CONST_INT_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<int>::ls_chunk, Method::NATIVE},
    });

	method("clear", {
		{Type::PTR_ARRAY, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_clear, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_clear, Method::NATIVE},
		{Type::INT_ARRAY, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_clear, Method::NATIVE},
	});

	method("contains", {
		{Type::BOOLEAN, {Type::CONST_ANY, Type::CONST_ANY}, (void*) &LSArray<LSValue*>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::CONST_ARRAY, Type::CONST_ANY}, (void*) &LSArray<LSValue*>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::CONST_REAL_ARRAY, Type::CONST_REAL}, (void*) &LSArray<double>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::CONST_INT_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<int>::ls_contains, Method::NATIVE},
	});

	Type iter_fun_type = Type::fun({}, {Type::ANY});
	Type iter_fun_type_int = Type::fun({}, {Type::INTEGER});
	Type iter_fun_type_float = Type::fun({}, {Type::REAL});
	Type iter_clo_type = Type::closure({}, {Type::ANY});
	Type iter_clo_type_int = Type::closure({}, {Type::INTEGER});
	Type iter_clo_type_float = Type::closure({}, {Type::REAL});
	auto iter_ptr = &LSArray<LSValue*>::ls_iter<LSFunction*>;
	auto iter_real = &LSArray<double>::ls_iter<LSFunction*>;
	auto iter_int = &LSArray<int>::ls_iter<LSFunction*>;
	auto iter_clo_ptr = &LSArray<LSValue*>::ls_iter<LSClosure*>;
	auto iter_clo_real = &LSArray<double>::ls_iter<LSClosure*>;
	auto iter_clo_int = &LSArray<int>::ls_iter<LSClosure*>;
	method("iter", {
		{{}, {Type::ARRAY, iter_fun_type}, (void*) iter_ptr, Method::NATIVE},
		{{}, {Type::ARRAY, iter_clo_type}, (void*) iter_clo_ptr, Method::NATIVE},
		{{}, {Type::REAL_ARRAY, iter_fun_type_float}, (void*) iter_real, Method::NATIVE},
		{{}, {Type::REAL_ARRAY, iter_clo_type_float}, (void*) iter_clo_real, Method::NATIVE},
		{{}, {Type::INT_ARRAY, iter_fun_type_int}, (void*) iter_int, Method::NATIVE},
		{{}, {Type::INT_ARRAY, iter_clo_type_int}, (void*) iter_clo_int, Method::NATIVE},
	});

	method("max", {
		{Type::ANY, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_max, Method::NATIVE},
		{Type::REAL, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_max, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_max, Method::NATIVE}
	});

	method("min", {
		{Type::ANY, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_min, Method::NATIVE},
		{Type::REAL, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_min, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_min, Method::NATIVE}
	});

	Type map_int_int_fun_type = Type::fun(Type::INTEGER, {Type::INTEGER});
	Type map_int_ptr_fun_type = Type::fun(Type::ANY, {Type::INTEGER});
	Type map_real_fun_type = Type::fun(Type::REAL, {Type::REAL});
	Type map_fun_type = Type::fun(Type::ANY, {Type::ANY});
	auto map_ptr = &LSArray<LSValue*>::ls_map<LSFunction*, LSValue*>;
	auto map_real = &LSArray<double>::ls_map<LSFunction*, double>;
	auto map_int_int = &LSArray<int>::ls_map<LSFunction*, int>;
	auto map_int_ptr = &LSArray<int>::ls_map<LSFunction*, LSValue*>;
	method("map", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, map_fun_type}, (void*) map_ptr, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY, map_real_fun_type}, (void*) map_real, Method::NATIVE},
		{Type::PTR_ARRAY_TMP, {Type::CONST_INT_ARRAY, map_int_ptr_fun_type}, (void*) map_int_ptr, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY, map_int_int_fun_type}, (void*) map_int_int, Method::NATIVE},
	});

	method("unique", {
		{Type::PTR_ARRAY_TMP, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_unique, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_unique, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_unique, Method::NATIVE},
	});

	method("sort", {
		{Type::PTR_ARRAY_TMP, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_sort, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_sort, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_sort, Method::NATIVE},
	});

	Type map2_fun_type = Type::fun(Type::ANY, {Type::ANY, Type::ANY});
	Type map2_fun_type_int = Type::fun(Type::ANY, {Type::ANY, Type::INTEGER});
	auto map2_ptr_ptr = &LSArray<LSValue*>::ls_map2<LSFunction*, LSValue*, LSValue*>;
	auto map2_ptr_int = &LSArray<LSValue*>::ls_map2<LSFunction*, LSValue*, int>;
	method("map2", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, Type::CONST_ARRAY, map2_fun_type}, (void*) map2_ptr_ptr, Method::NATIVE},
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, Type::CONST_INT_ARRAY, map2_fun_type_int}, (void*) map2_ptr_int, Method::NATIVE},
	});

	Type pred_fun_type = Type::fun(Type::BOOLEAN, {Type::ANY});
	Type pred_fun_type_float = Type::fun(Type::BOOLEAN, {Type::REAL});
	Type pred_fun_type_int = Type::fun(Type::BOOLEAN, {Type::INTEGER});
	Type pred_clo_type = Type::closure(Type::BOOLEAN, {Type::ANY});
	Type pred_clo_type_float = Type::closure(Type::BOOLEAN, {Type::REAL});
	Type pred_clo_type_int = Type::closure(Type::BOOLEAN, {Type::INTEGER});
	auto filter_ptr = &LSArray<LSValue*>::ls_filter<LSFunction*>;
	auto filter_real = &LSArray<double>::ls_filter<LSFunction*>;
	auto filter_int = &LSArray<int>::ls_filter<LSFunction*>;
	auto filter_clo_ptr = &LSArray<LSValue*>::ls_filter<LSClosure*>;
	auto filter_clo_real = &LSArray<double>::ls_filter<LSClosure*>;
	auto filter_clo_int = &LSArray<int>::ls_filter<LSClosure*>;
	method("filter", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, pred_fun_type}, (void*) filter_ptr, Method::NATIVE},
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, pred_clo_type}, (void*) filter_clo_ptr, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY, pred_fun_type_float}, (void*) filter_real, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY, pred_clo_type_float}, (void*) filter_clo_real, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY, pred_fun_type_int}, (void*) filter_int, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY, pred_clo_type_int}, (void*) filter_clo_int, Method::NATIVE}
	});

	method("isEmpty", {
		{Type::BOOLEAN, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_empty, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_empty, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_empty, Method::NATIVE},
	});

	auto perm_int_int = &LSArray<int>::is_permutation<int>;
	auto perm_int_real = &LSArray<int>::is_permutation<double>;
	auto perm_int_ptr = &LSArray<int>::is_permutation<LSValue*>;
	auto perm_real_int = &LSArray<double>::is_permutation<int>;
	auto perm_real_real = &LSArray<double>::is_permutation<double>;
	auto perm_real_ptr = &LSArray<double>::is_permutation<LSValue*>;
	auto perm_ptr_int = &LSArray<LSValue*>::is_permutation<int>;
	auto perm_ptr_real = &LSArray<LSValue*>::is_permutation<double>;
	auto perm_ptr_ptr = &LSArray<LSValue*>::is_permutation<LSValue*>;
	method("isPermutation", {
		{Type::BOOLEAN, {Type::ARRAY, Type::ARRAY}, (void*) perm_ptr_ptr, Method::NATIVE},
		{Type::BOOLEAN, {Type::ARRAY, Type::REAL_ARRAY}, (void*) perm_ptr_real, Method::NATIVE},
		{Type::BOOLEAN, {Type::ARRAY, Type::INT_ARRAY}, (void*) perm_ptr_int, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::PTR_ARRAY}, (void*) perm_real_ptr, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::REAL_ARRAY}, (void*) perm_real_real, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::INT_ARRAY}, (void*) perm_real_int, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::PTR_ARRAY}, (void*) perm_int_ptr, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::REAL_ARRAY}, (void*) perm_int_real, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INT_ARRAY}, (void*) perm_int_int, Method::NATIVE},
	});

	auto partition_ptr = &LSArray<LSValue*>::ls_partition<LSFunction*>;
	auto partition_real = &LSArray<double>::ls_partition<LSFunction*>;
	auto partition_int = &LSArray<int>::ls_partition<LSFunction*>;
	method("partition", {
		{Type::PTR_ARRAY_TMP, {Type::ARRAY, pred_fun_type}, (void*) partition_ptr, Method::NATIVE},
		{Type::PTR_ARRAY_TMP, {Type::REAL_ARRAY, pred_fun_type_float}, (void*) partition_real, Method::NATIVE},
		{Type::PTR_ARRAY_TMP, {Type::INT_ARRAY, pred_fun_type_int}, (void*) partition_int, Method::NATIVE},
	});

	method("first", {
		{Type::ANY, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_first, Method::NATIVE},
		{Type::REAL, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_first, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_first, Method::NATIVE},
	});

	method("last", {
		{Type::ANY, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_last, Method::NATIVE},
		{Type::REAL, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_last, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_last, Method::NATIVE},
	});

	Type fold_fun_type = Type::fun(Type::ANY, {Type::ANY, Type::ANY});
	Type fold_fun_type_float = Type::fun(Type::ANY, {Type::ANY, Type::REAL});
	Type fold_fun_type_int = Type::fun(Type::ANY, {Type::ANY, Type::INTEGER});
	Type fold_clo_type = Type::closure(Type::ANY, {Type::ANY, Type::ANY});
	Type fold_clo_type_float = Type::closure(Type::ANY, {Type::ANY, Type::REAL});
	Type fold_clo_type_int = Type::closure(Type::ANY, {Type::ANY, Type::INTEGER});
	auto fold_left_ptr = &LSArray<LSValue*>::ls_foldLeft<LSFunction*, LSValue*>;
	auto fold_left_int = &LSArray<int>::ls_foldLeft<LSFunction*, LSValue*>;
	auto fold_left_real = &LSArray<double>::ls_foldLeft<LSFunction*, LSValue*>;
	auto fold_left_clo_ptr = &LSArray<LSValue*>::ls_foldLeft<LSClosure*, LSValue*>;
	auto fold_left_clo_int = &LSArray<int>::ls_foldLeft<LSClosure*, LSValue*>;
	auto fold_left_clo_real = &LSArray<double>::ls_foldLeft<LSClosure*, LSValue*>;
	method("foldLeft", {
		{Type::ANY, {Type::ARRAY, fold_fun_type, Type::ANY}, (void*) fold_left_ptr, Method::NATIVE},
		{Type::ANY, {Type::ARRAY, fold_clo_type, Type::ANY}, (void*) fold_left_clo_ptr, Method::NATIVE},
		{Type::ANY, {Type::REAL_ARRAY, fold_fun_type_float, Type::ANY}, (void*) fold_left_real, Method::NATIVE},
		{Type::ANY, {Type::REAL_ARRAY, fold_clo_type_float, Type::ANY}, (void*) fold_left_clo_real, Method::NATIVE},
		{Type::ANY, {Type::INT_ARRAY, fold_fun_type_int, Type::ANY}, (void*) fold_left_int, Method::NATIVE},
		{Type::ANY, {Type::INT_ARRAY, fold_clo_type_int, Type::ANY}, (void*) fold_left_clo_int, Method::NATIVE}
	});

	Type fold_right_fun_type = Type::fun(Type::ANY, {Type::ANY, Type::ANY});
	Type fold_right_fun_type_float = Type::fun(Type::ANY, {Type::REAL, Type::ANY});
	Type fold_right_fun_type_int = Type::fun(Type::ANY, {Type::INTEGER, Type::ANY});
	Type fold_right_clo_type = Type::closure(Type::ANY, {Type::ANY, Type::ANY});
	Type fold_right_clo_type_float = Type::closure(Type::ANY, {Type::REAL, Type::ANY});
	Type fold_right_clo_type_int = Type::closure(Type::ANY, {Type::INTEGER, Type::ANY});

	auto fold_right_ptr = &LSArray<LSValue*>::ls_foldRight<LSFunction*, LSValue*>;
	auto fold_right_int = &LSArray<int>::ls_foldRight<LSFunction*, LSValue*>;
	auto fold_right_real = &LSArray<double>::ls_foldRight<LSFunction*, LSValue*>;
	auto fold_right_clo_ptr = &LSArray<LSValue*>::ls_foldRight<LSClosure*, LSValue*>;
	auto fold_right_clo_int = &LSArray<int>::ls_foldRight<LSClosure*, LSValue*>;
	auto fold_right_clo_real = &LSArray<double>::ls_foldRight<LSClosure*, LSValue*>;

	method("foldRight", {
		{Type::ANY, {Type::ARRAY, fold_right_fun_type, Type::ANY}, (void*) fold_right_ptr, Method::NATIVE},
		{Type::ANY, {Type::ARRAY, fold_right_clo_type, Type::ANY}, (void*) fold_right_clo_ptr, Method::NATIVE},
		{Type::ANY, {Type::REAL_ARRAY, fold_right_fun_type_float, Type::ANY}, (void*) fold_right_real, Method::NATIVE},
		{Type::ANY, {Type::REAL_ARRAY, fold_right_clo_type_float, Type::ANY}, (void*) fold_right_clo_real, Method::NATIVE},
		{Type::ANY, {Type::INT_ARRAY, fold_right_fun_type_int, Type::ANY}, (void*) fold_right_int, Method::NATIVE},
		{Type::ANY, {Type::INT_ARRAY, fold_right_clo_type_int, Type::ANY}, (void*) fold_right_clo_int, Method::NATIVE},
	});

	method("pop", {
		{Type::ANY, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_pop, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_pop, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_pop, Method::NATIVE},
	});

	method("product", {
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_product, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_product, Method::NATIVE}
	});

	method("push", {
		{Type::PTR_ARRAY, {Type::ANY, Type::CONST_ANY}, (void*) &LSArray<LSValue*>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::PTR_ARRAY, {Type::ARRAY, Type::CONST_ANY}, (void*) &LSArray<LSValue*>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::CONST_REAL}, (void*) &LSArray<double>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<int>::ls_push, Method::NATIVE, {new WillStoreMutator()}}
	});

	method("pushAll", {
		{Type::PTR_ARRAY, {Type::ARRAY, Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::ARRAY, Type::REAL_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_flo, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::ARRAY, Type::INT_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_int, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::REAL_ARRAY, Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE, {new WillStoreMutator()}},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_push_all_flo, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::INT_ARRAY}, (void*) &LSArray<double>::ls_push_all_int, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::INT_ARRAY, Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE, {new WillStoreMutator()}},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::REAL_ARRAY}, (void*) &LSArray<int>::ls_push_all_flo, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INT_ARRAY}, (void*) &LSArray<int>::ls_push_all_int, Method::NATIVE},
	});

	method("join", {
		{Type::STRING, {Type::CONST_ARRAY, Type::CONST_STRING}, (void*) &LSArray<LSValue*>::ls_join, Method::NATIVE},
		{Type::STRING, {Type::CONST_REAL_ARRAY, Type::CONST_STRING}, (void*) &LSArray<double>::ls_join, Method::NATIVE},
		{Type::STRING, {Type::CONST_INT_ARRAY, Type::CONST_STRING}, (void*) &LSArray<int>::ls_join, Method::NATIVE},
	});

	method("json", {
		{Type::STRING, {Type::ARRAY}, (void*) &LSValue::ls_json, Method::NATIVE},
	});

	method("fill", {
		{Type::PTR_ARRAY, {Type::ARRAY, Type::ANY, Type::CONST_INTEGER}, (void*) &LSArray<LSValue*>::ls_fill, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL, Type::CONST_INTEGER}, (void*) &LSArray<double>::ls_fill, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::CONST_INTEGER}, (void*) &LSArray<int>::ls_fill, Method::NATIVE},
	});

	method("insert", {
		{Type::PTR_ARRAY, {Type::ARRAY, Type::ANY, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_insert, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_insert, Method::NATIVE},
	});

	method("random", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<LSValue*>::ls_random, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<double>::ls_random, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY, Type::CONST_INTEGER}, (void*) &LSArray<int>::ls_random, Method::NATIVE},
	});

	method("remove", {
		{Type::ANY, {Type::ARRAY, Type::INTEGER}, (void*)&LSArray<LSValue*>::ls_remove, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY, Type::INTEGER}, (void*)&LSArray<double>::ls_remove, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER}, (void*)&LSArray<int>::ls_remove, Method::NATIVE},
	});

	method("removeElement", {
		{Type::BOOLEAN, {Type::ARRAY, Type::CONST_ANY}, (void*)&LSArray<LSValue*>::ls_remove_element, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::CONST_REAL}, (void*)&LSArray<double>::ls_remove_element, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::CONST_INTEGER}, (void*)&LSArray<int>::ls_remove_element, Method::NATIVE},
	});

	method("reverse", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_reverse, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_reverse, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_reverse, Method::NATIVE},
	});

	method("shuffle", {
		{Type::PTR_ARRAY, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_shuffle, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_shuffle, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_shuffle, Method::NATIVE},
	});

	method("search", {
		{Type::INTEGER, {Type::CONST_ARRAY, Type::CONST_ANY, Type::CONST_INTEGER}, (void*) &LSArray<LSValue*>::ls_search, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_REAL_ARRAY, Type::CONST_REAL, Type::CONST_INTEGER}, (void*) &LSArray<double>::ls_search, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_ARRAY, Type::CONST_INTEGER, Type::CONST_INTEGER}, (void*) &LSArray<int>::ls_search, Method::NATIVE},
	});

	method("size", {
		{Type::ANY, {Type::CONST_ANY}, (void*) &LSArray<LSValue*>::ls_size_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_ANY}, (void*) &ArraySTD::size},
		{Type::INTEGER, {Type::CONST_REAL_ARRAY}, (void*) &ArraySTD::size},
		{Type::INTEGER, {Type::CONST_INT_ARRAY}, (void*) &ArraySTD::size}
	});

	method("sum", {
		{Type::ANY, {Type::CONST_ARRAY}, (void*) &LSArray<LSValue*>::ls_sum, Method::NATIVE},
		{Type::REAL, {Type::CONST_REAL_ARRAY}, (void*) &LSArray<double>::ls_sum, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_ARRAY}, (void*) &LSArray<int>::ls_sum, Method::NATIVE},
	});

	method("subArray", {
		{Type::PTR_ARRAY_TMP, {Type::CONST_ARRAY, Type::CONST_INTEGER, Type::CONST_INTEGER}, (void* ) &ArraySTD::sub, Method::NATIVE},
		{Type::REAL_ARRAY_TMP, {Type::CONST_REAL_ARRAY, Type::CONST_INTEGER, Type::CONST_INTEGER}, (void* ) &ArraySTD::sub, Method::NATIVE},
		{Type::INT_ARRAY_TMP, {Type::CONST_INT_ARRAY, Type::CONST_INTEGER, Type::CONST_INTEGER}, (void* ) &ArraySTD::sub, Method::NATIVE},
	});
}

Compiler::value ArraySTD::array_add_eq(Compiler& c, std::vector<Compiler::value> args) {
	args[1] = c.insn_to_any(args[1]);
	return c.insn_call(Type::ANY, args, (void*) +[](LSValue** x, LSValue* y) {
		return (*x)->add_eq(y);
	});
}

Compiler::value ArraySTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, LSValue* b) {
		return a->lt(b);
	});
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ArraySTD::size(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_array_size(args[0]);
	c.insn_delete_temporary(args[0]);
	return res;
}

LSArray<LSValue*>* ArraySTD::chunk_1_ptr(LSArray<LSValue*>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* ArraySTD::chunk_1_int(LSArray<int>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* ArraySTD::chunk_1_float(LSArray<double>* array) {
	return array->ls_chunk(1);
}

LSValue* ArraySTD::sub(LSArray<LSValue*>* array, int begin, int end) {
	LSValue* r = array->range(begin, end);
	if (array->refs == 0) delete array;
	return r;
}

}
