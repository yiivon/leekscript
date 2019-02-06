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
		{Type::const_array(), Type::const_any(), Type::boolean(), (void*) &in},
	});
	operator_("+", {
		{Type::const_array(), Type::const_any(), Type::array(Type::any()), (void*) &ArraySTD::op_add},
	});
	operator_("+=", {
		{Type::array(), Type::const_any(), Type::any(), (void*) &array_add_eq, {new WillStoreMutator()}, false, true},
		{Type::array(Type::real()), Type::const_real(), Type::array(Type::real()), (void*) &LSArray<double>::add_eq_double, {new WillStoreMutator()}, Method::NATIVE},
		{Type::array(Type::integer()), Type::const_integer(), Type::array(Type::integer()), (void*) &LSArray<int>::add_eq_int, {new WillStoreMutator()}, Method::NATIVE}
	});

	/* Type tilde_tilde_fun_type = Type::FUNCTION;
	tilde_tilde_fun_type.setArgumentType(0, Type::T);
	tilde_tilde_fun_type.setReturnType(Type::any());
	operator_("~~", {
		{Type::T_ARRAY, tilde_tilde_fun_type, Type::array(Type::any()), (void*) &LSArray<LSValue*>::ls_map}
	});*/

	/*
	 * Methods
	 */
	method("copy", {
		{Type::tmp_array(), {Type::const_array()}, (void*) &ValueSTD::copy},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real())}, (void*) &ValueSTD::copy},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer())}, (void*) &ValueSTD::copy},
	});
	method("average", {
		{Type::real(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_average, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_average, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_average, Method::NATIVE},
	});

	method("chunk", {
		{Type::array(Type::array()), {Type::const_array()}, (void*) ArraySTD::chunk_1_ptr, Method::NATIVE},
		{Type::array(Type::array(Type::real())), {Type::const_array(Type::real())}, (void*) ArraySTD::chunk_1_float, Method::NATIVE},
		{Type::array(Type::array(Type::integer())), {Type::const_array(Type::integer())}, (void*) ArraySTD::chunk_1_int, Method::NATIVE},
		{Type::array(Type::array()), {Type::const_array(), Type::const_integer()}, (void*) &LSArray<LSValue*>::ls_chunk, Method::NATIVE},
		{Type::array(Type::array(Type::real())), {Type::const_array(Type::real()), Type::const_integer()}, (void*) &LSArray<double>::ls_chunk, Method::NATIVE},
		{Type::array(Type::array(Type::integer())), {Type::const_array(Type::integer()), Type::const_integer()}, (void*) &LSArray<int>::ls_chunk, Method::NATIVE},
    });

	method("clear", {
		{Type::array(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_clear, Method::NATIVE},
		{Type::array(Type::real()), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_clear, Method::NATIVE},
		{Type::array(Type::integer()), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_clear, Method::NATIVE},
	});

	method("contains", {
		{Type::boolean(), {Type::const_array(), Type::const_any()}, (void*) &ArraySTD::contains},
		{Type::boolean(), {Type::const_array(Type::real()), Type::const_real()}, (void*) &LSArray<double>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::const_array(Type::integer()), Type::const_integer()}, (void*) &LSArray<int>::ls_contains, Method::NATIVE},
	});

	Type iter_fun_type = Type::fun({}, {Type::any()});
	Type iter_fun_type_int = Type::fun({}, {Type::integer()});
	Type iter_fun_type_float = Type::fun({}, {Type::real()});
	Type iter_clo_type = Type::closure({}, {Type::any()});
	Type iter_clo_type_int = Type::closure({}, {Type::integer()});
	Type iter_clo_type_float = Type::closure({}, {Type::real()});
	auto iter_ptr = &LSArray<LSValue*>::ls_iter<LSFunction*>;
	auto iter_real = &LSArray<double>::ls_iter<LSFunction*>;
	auto iter_int = &LSArray<int>::ls_iter<LSFunction*>;
	auto iter_clo_ptr = &LSArray<LSValue*>::ls_iter<LSClosure*>;
	auto iter_clo_real = &LSArray<double>::ls_iter<LSClosure*>;
	auto iter_clo_int = &LSArray<int>::ls_iter<LSClosure*>;
	method("iter", {
		{{}, {Type::array(), iter_fun_type}, (void*) iter_ptr, Method::NATIVE},
		{{}, {Type::array(), iter_clo_type}, (void*) iter_clo_ptr, Method::NATIVE},
		{{}, {Type::array(Type::real()), iter_fun_type_float}, (void*) iter_real, Method::NATIVE},
		{{}, {Type::array(Type::real()), iter_clo_type_float}, (void*) iter_clo_real, Method::NATIVE},
		{{}, {Type::array(Type::integer()), iter_fun_type_int}, (void*) iter_int, Method::NATIVE},
		{{}, {Type::array(Type::integer()), iter_clo_type_int}, (void*) iter_clo_int, Method::NATIVE},
	});

	method("max", {
		{Type::any(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_max, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_max, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_max, Method::NATIVE}
	});

	method("min", {
		{Type::any(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_min, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_min, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_min, Method::NATIVE}
	});

	Type map_int_int_fun_type = Type::fun(Type::integer(), {Type::integer()});
	Type map_int_ptr_fun_type = Type::fun(Type::any(), {Type::integer()});
	Type map_real_fun_type = Type::fun(Type::real(), {Type::real()});
	Type map_fun_type = Type::fun(Type::any(), {Type::any()});
	auto map_fun = &LSArray<LSValue*>::ls_map<LSFunction*, LSValue*>;
	method("map", {
		{Type::tmp_array(), {Type::const_array(), map_fun_type}, (void*) map_fun, Method::NATIVE},
		{Type::tmp_array(), {Type::const_array(), map_fun_type}, (void*) map},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real()), map_real_fun_type}, (void*) map},
		{Type::tmp_array(), {Type::const_array(Type::integer()), map_int_ptr_fun_type}, (void*) map},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer()), map_int_int_fun_type}, (void*) map},
	});
	// auto map_fun = &LSArray<LSValue*>::ls_map<LSFunction*, LSValue*>;
	// auto E = Type::template_("E");
	// auto R = Type::template_("R");
	// template_(E, R).method("map", {
	// 	{Type::tmp_array(R), {Type::const_array(E), Type::fun(R, {E})}, (void*) map_fun, Method::NATIVE, {
	// 		new WillTakeMutator(1, {E})
	// 	}},
	// });

	method("unique", {
		{Type::tmp_array(), {Type::array()}, (void*) &LSArray<LSValue*>::ls_unique, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_unique, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_unique, Method::NATIVE},
	});

	method("sort", {
		{Type::tmp_array(), {Type::array()}, (void*) &LSArray<LSValue*>::ls_sort, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_sort, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_sort, Method::NATIVE},
	});

	Type map2_fun_type = Type::fun(Type::any(), {Type::any(), Type::any()});
	Type map2_fun_type_int = Type::fun(Type::any(), {Type::any(), Type::integer()});
	auto map2_ptr_ptr = &LSArray<LSValue*>::ls_map2<LSFunction*, LSValue*, LSValue*>;
	auto map2_ptr_int = &LSArray<LSValue*>::ls_map2<LSFunction*, LSValue*, int>;
	method("map2", {
		{Type::tmp_array(), {Type::const_array(), Type::const_array(), map2_fun_type}, (void*) map2_ptr_ptr, Method::NATIVE},
		{Type::tmp_array(), {Type::const_array(), Type::const_array(Type::integer()), map2_fun_type_int}, (void*) map2_ptr_int, Method::NATIVE},
	});

	Type pred_fun_type = Type::fun(Type::any(), {Type::any()});
	Type pred_fun_type_float = Type::fun(Type::any(), {Type::real()});
	Type pred_fun_type_int = Type::fun(Type::any(), {Type::integer()});
	Type pred_clo_type = Type::closure(Type::any(), {Type::any()});
	Type pred_clo_type_float = Type::closure(Type::any(), {Type::real()});
	Type pred_clo_type_int = Type::closure(Type::any(), {Type::integer()});
	auto filter_ptr = &LSArray<LSValue*>::ls_filter<LSFunction*>;
	auto filter_real = &LSArray<double>::ls_filter<LSFunction*>;
	auto filter_int = &LSArray<int>::ls_filter<LSFunction*>;
	auto filter_clo_ptr = &LSArray<LSValue*>::ls_filter<LSClosure*>;
	auto filter_clo_real = &LSArray<double>::ls_filter<LSClosure*>;
	auto filter_clo_int = &LSArray<int>::ls_filter<LSClosure*>;
	method("filter", {
		{Type::tmp_array(), {Type::const_array(), pred_fun_type}, (void*) filter_ptr, Method::NATIVE},
		{Type::tmp_array(), {Type::const_array(), pred_clo_type}, (void*) filter_clo_ptr, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real()), pred_fun_type_float}, (void*) filter_real, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real()), pred_clo_type_float}, (void*) filter_clo_real, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer()), pred_fun_type_int}, (void*) filter_int, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer()), pred_clo_type_int}, (void*) filter_clo_int, Method::NATIVE}
	});

	method("isEmpty", {
		{Type::boolean(), {Type::array()}, (void*) &LSArray<LSValue*>::ls_empty, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_empty, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_empty, Method::NATIVE},
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
		{Type::boolean(), {Type::array(), Type::array()}, (void*) perm_ptr_ptr, Method::NATIVE},
		{Type::boolean(), {Type::array(), Type::array(Type::real())}, (void*) perm_ptr_real, Method::NATIVE},
		{Type::boolean(), {Type::array(), Type::array(Type::integer())}, (void*) perm_ptr_int, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::real()), Type::array(Type::any())}, (void*) perm_real_ptr, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::real()), Type::array(Type::real())}, (void*) perm_real_real, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::real()), Type::array(Type::integer())}, (void*) perm_real_int, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::integer()), Type::array(Type::any())}, (void*) perm_int_ptr, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::integer()), Type::array(Type::real())}, (void*) perm_int_real, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::integer()), Type::array(Type::integer())}, (void*) perm_int_int, Method::NATIVE},
	});

	method("partition", {
		{Type::tmp_array(), {Type::array(), pred_fun_type}, (void*) partition},
		{Type::tmp_array(), {Type::array(Type::real()), pred_fun_type_float}, (void*) partition},
		{Type::tmp_array(), {Type::array(Type::integer()), pred_fun_type_int}, (void*) partition},
	});

	method("first", {
		{Type::any(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_first, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_first, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_first, Method::NATIVE},
	});

	method("last", {
		{Type::any(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_last, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_last, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_last, Method::NATIVE},
	});

	Type fold_fun_type = Type::fun(Type::any(), {Type::any(), Type::any()});
	Type fold_fun_type_float = Type::fun(Type::any(), {Type::any(), Type::real()});
	Type fold_fun_type_int = Type::fun(Type::any(), {Type::any(), Type::integer()});
	Type fold_clo_type = Type::closure(Type::any(), {Type::any(), Type::any()});
	Type fold_clo_type_float = Type::closure(Type::any(), {Type::any(), Type::real()});
	Type fold_clo_type_int = Type::closure(Type::any(), {Type::any(), Type::integer()});
	method("foldLeft", {
		{Type::any(), {Type::array(), fold_fun_type, Type::any()}, (void*) &fold_left_ptr},
		{Type::any(), {Type::array(), fold_clo_type, Type::any()}, (void*) &fold_left_clo_ptr},
		{Type::any(), {Type::array(Type::real()), fold_fun_type_float, Type::any()}, (void*) &fold_left_real},
		{Type::any(), {Type::array(Type::real()), fold_clo_type_float, Type::any()}, (void*) &fold_left_clo_real},
		{Type::any(), {Type::array(Type::integer()), fold_fun_type_int, Type::any()}, (void*) &fold_left_int},
		{Type::any(), {Type::array(Type::integer()), fold_clo_type_int, Type::any()}, (void*) &fold_left_clo_int},
	});

	Type fold_right_fun_type = Type::fun(Type::any(), {Type::any(), Type::any()});
	Type fold_right_fun_type_float = Type::fun(Type::any(), {Type::real(), Type::any()});
	Type fold_right_fun_type_int = Type::fun(Type::any(), {Type::integer(), Type::any()});
	Type fold_right_clo_type = Type::closure(Type::any(), {Type::any(), Type::any()});
	Type fold_right_clo_type_float = Type::closure(Type::any(), {Type::real(), Type::any()});
	Type fold_right_clo_type_int = Type::closure(Type::any(), {Type::integer(), Type::any()});

	method("foldRight", {
		{Type::any(), {Type::array(), fold_right_fun_type, Type::any()}, (void*) &fold_right_ptr},
		{Type::any(), {Type::array(), fold_right_clo_type, Type::any()}, (void*) &fold_right_clo_ptr},
		{Type::any(), {Type::array(Type::real()), fold_right_fun_type_float, Type::any()}, (void*) &fold_right_real},
		{Type::any(), {Type::array(Type::real()), fold_right_clo_type_float, Type::any()}, (void*) &fold_right_clo_real},
		{Type::any(), {Type::array(Type::integer()), fold_right_fun_type_int, Type::any()}, (void*) &fold_right_int},
		{Type::any(), {Type::array(Type::integer()), fold_right_clo_type_int, Type::any()}, (void*) &fold_right_clo_int},
	});

	method("pop", {
		{Type::any(), {Type::array()}, (void*) &LSArray<LSValue*>::ls_pop, Method::NATIVE},
		{Type::real(), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_pop, Method::NATIVE},
		{Type::integer(), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_pop, Method::NATIVE},
	});

	method("product", {
		{Type::real(), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_product, Method::NATIVE},
		{Type::integer(), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_product, Method::NATIVE}
	});

	method("push", {
		{Type::array(Type::any()), {Type::array(), Type::const_any()}, (void*) &LSArray<LSValue*>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::array(Type::real()), {Type::array(Type::real()), Type::const_real()}, (void*) &LSArray<double>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::array(Type::real()), {Type::array(Type::integer()), Type::const_real()}, (void*) &LSArray<double>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::array(Type::integer()), {Type::array(Type::integer()), Type::const_integer()}, (void*) &LSArray<int>::ls_push, Method::NATIVE, {new WillStoreMutator()}}
	});

	method("pushAll", {
		{Type::array(Type::any()), {Type::array(), Type::array()}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE},
		{Type::array(Type::any()), {Type::array(), Type::array(Type::real())}, (void*) &LSArray<LSValue*>::ls_push_all_flo, Method::NATIVE},
		{Type::array(Type::any()), {Type::array(), Type::array(Type::integer())}, (void*) &LSArray<LSValue*>::ls_push_all_int, Method::NATIVE},
		{Type::array(Type::any()), {Type::array(Type::real()), Type::array()}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE, {new WillStoreMutator()}},
		{Type::array(Type::real()), {Type::array(Type::real()), Type::array(Type::real())}, (void*) &LSArray<double>::ls_push_all_flo, Method::NATIVE},
		{Type::array(Type::real()), {Type::array(Type::real()), Type::array(Type::integer())}, (void*) &LSArray<double>::ls_push_all_int, Method::NATIVE},
		{Type::array(Type::any()), {Type::array(Type::integer()), Type::array()}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE, {new WillStoreMutator()}},
		{Type::array(Type::integer()), {Type::array(Type::integer()), Type::array(Type::real())}, (void*) &LSArray<int>::ls_push_all_flo, Method::NATIVE},
		{Type::array(Type::integer()), {Type::array(Type::integer()), Type::array(Type::integer())}, (void*) &LSArray<int>::ls_push_all_int, Method::NATIVE},
	});

	method("join", {
		{Type::string(), {Type::const_array(), Type::const_string()}, (void*) &LSArray<LSValue*>::ls_join, Method::NATIVE},
		{Type::string(), {Type::const_array(Type::real()), Type::const_string()}, (void*) &LSArray<double>::ls_join, Method::NATIVE},
		{Type::string(), {Type::const_array(Type::integer()), Type::const_string()}, (void*) &LSArray<int>::ls_join, Method::NATIVE},
	});

	method("json", {
		{Type::string(), {Type::array()}, (void*) &LSValue::ls_json, Method::NATIVE},
	});

	// template<T> array<T> fill(array, T, integer)
	method("fill", {
		{Type::array(Type::any()), {Type::array(), Type::any(), Type::const_integer()}, (void*) &fill, false, {new WillStoreMutator()}},
		{Type::array(Type::real()), {Type::array(), Type::real(), Type::const_integer()}, (void*) &fill, false, {new WillStoreMutator()}},
		{Type::array(Type::integer()), {Type::array(), Type::integer(), Type::const_integer()}, (void*) &fill, false, {new WillStoreMutator()}},
	});

	method("insert", {
		{Type::array(Type::any()), {Type::array(), Type::any(), Type::integer()}, (void*) &LSArray<LSValue*>::ls_insert, Method::NATIVE},
		{Type::array(Type::real()), {Type::array(Type::real()), Type::real(), Type::integer()}, (void*) &LSArray<double>::ls_insert, Method::NATIVE},
		{Type::array(Type::integer()), {Type::array(Type::integer()), Type::integer(), Type::integer()}, (void*) &LSArray<int>::ls_insert, Method::NATIVE},
	});

	method("random", {
		{Type::tmp_array(), {Type::const_array(), Type::const_integer()}, (void*) &LSArray<LSValue*>::ls_random, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real()), Type::const_integer()}, (void*) &LSArray<double>::ls_random, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer()), Type::const_integer()}, (void*) &LSArray<int>::ls_random, Method::NATIVE},
	});

	method("remove", {
		{Type::any(), {Type::array(), Type::integer()}, (void*)&LSArray<LSValue*>::ls_remove, Method::NATIVE},
		{Type::real(), {Type::array(Type::real()), Type::integer()}, (void*)&LSArray<double>::ls_remove, Method::NATIVE},
		{Type::integer(), {Type::array(Type::integer()), Type::integer()}, (void*)&LSArray<int>::ls_remove, Method::NATIVE},
	});

	method("removeElement", {
		{Type::boolean(), {Type::array(), Type::const_any()}, (void*) &remove_element_any},
		{Type::boolean(), {Type::array(Type::real()), Type::const_any()}, (void*) &remove_element_real},
		{Type::boolean(), {Type::array(Type::integer()), Type::const_any()}, (void*) &remove_element_int},
	});

	method("reverse", {
		{Type::tmp_array(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_reverse, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_reverse, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_reverse, Method::NATIVE},
	});

	method("shuffle", {
		{Type::array(Type::any()), {Type::array()}, (void*) &LSArray<LSValue*>::ls_shuffle, Method::NATIVE},
		{Type::array(Type::real()), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_shuffle, Method::NATIVE},
		{Type::array(Type::integer()), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_shuffle, Method::NATIVE},
	});

	method("search", {
		{Type::integer(), {Type::const_array(), Type::const_any(), Type::const_integer()}, (void*) &ArraySTD::search_any},
		{Type::integer(), {Type::const_array(Type::real()), Type::const_real(), Type::const_integer()}, (void*) &ArraySTD::search_real},
		{Type::integer(), {Type::const_array(Type::integer()), Type::const_integer(), Type::const_integer()}, (void*) &ArraySTD::search_int},
	});

	method("size", {
		{Type::any(), {Type::const_any()}, (void*) &LSArray<LSValue*>::ls_size_ptr, Method::NATIVE},
		{Type::integer(), {Type::const_any()}, (void*) &ArraySTD::size},
		{Type::integer(), {Type::const_array(Type::real())}, (void*) &ArraySTD::size},
		{Type::integer(), {Type::const_array(Type::integer())}, (void*) &ArraySTD::size}
	});

	method("sum", {
		{Type::any(), {Type::const_array()}, (void*) &LSArray<LSValue*>::ls_sum, Method::NATIVE},
		{Type::real(), {Type::const_array(Type::real())}, (void*) &LSArray<double>::ls_sum, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::integer())}, (void*) &LSArray<int>::ls_sum, Method::NATIVE},
	});

	method("subArray", {
		{Type::tmp_array(), {Type::const_array(), Type::const_integer(), Type::const_integer()}, (void* ) &ArraySTD::sub, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::const_array(Type::real()), Type::const_integer(), Type::const_integer()}, (void* ) &ArraySTD::sub, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::const_array(Type::integer()), Type::const_integer(), Type::const_integer()}, (void* ) &ArraySTD::sub, Method::NATIVE},
	});
}

Compiler::value ArraySTD::in(Compiler& c, std::vector<Compiler::value> args) {
	const auto& type = args[0].t.element().fold();
	auto f = [&]() {
		if (type.is_integer()) return (void*) &LSArray<int>::in_i;
		if (type.is_integer()) return (void*) &LSArray<double>::in;
		return (void*) &LSArray<LSValue*>::in;
	}();
	if (args[1].t.castable(type)) {
		auto v = c.insn_convert(args[1], type);
		auto r = c.insn_invoke(Type::boolean(), {args[0], v}, f);
		if (args[1].t.is_polymorphic() and type.is_primitive()) {
			c.insn_delete_temporary(args[1]);
		}
		return r;
	} else {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
}

Compiler::value ArraySTD::op_add(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::array(args[0].t.element()), {args[0], c.insn_to_any(args[1])}, +[](LSValue* x, LSValue* y) {
		return x->add(y);
	});
}

Compiler::value ArraySTD::array_add_eq(Compiler& c, std::vector<Compiler::value> args) {
	args[1] = c.insn_to_any(args[1]);
	return c.insn_call(Type::any(), args, (void*) +[](LSValue** x, LSValue* y) {
		return (*x)->add_eq(y);
	});
}

Compiler::value ArraySTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_call(Type::boolean(), args, +[](LSValue* a, LSValue* b) {
		return a->lt(b);
	});
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ArraySTD::contains(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1])}, +[](LSArray<LSValue*>* array, LSValue* v) {
		return array->ls_contains(v);
	});
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

Compiler::value ArraySTD::search_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1]), c.to_int(args[2])}, (void*) &LSArray<LSValue*>::ls_search);
}
Compiler::value ArraySTD::search_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_real(args[1]), c.to_int(args[2])}, (void*) &LSArray<double>::ls_search);
}
Compiler::value ArraySTD::search_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_int(args[1]), c.to_int(args[2])}, (void*) &LSArray<int>::ls_search);
}

Compiler::value ArraySTD::fill(Compiler& c, std::vector<Compiler::value> args) {
	auto fun = [&]() {
		if (args[0].t.element().is_integer()) return (void*) &LSArray<int>::ls_fill;
		if (args[0].t.element().is_real()) return (void*) &LSArray<double>::ls_fill;
		return (void*) &LSArray<LSValue*>::ls_fill;
	}();
	return c.insn_call(args[0].t, {args[0], c.insn_convert(args[1], args[0].t.element()), c.to_int(args[2])}, fun);
}

Compiler::value ArraySTD::fold_left_ptr(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_left_ptr = &LSArray<LSValue*>::ls_foldLeft<LSFunction*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_left_ptr);
}
Compiler::value ArraySTD::fold_left_clo_ptr(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_left_clo_ptr = &LSArray<LSValue*>::ls_foldLeft<LSClosure*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_left_clo_ptr);
}
Compiler::value ArraySTD::fold_left_real(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_left_real = &LSArray<double>::ls_foldLeft<LSFunction*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_left_real);
}
Compiler::value ArraySTD::fold_left_clo_real(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_left_clo_real = &LSArray<double>::ls_foldLeft<LSClosure*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_left_clo_real);
}
Compiler::value ArraySTD::fold_left_int(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_left_int = &LSArray<int>::ls_foldLeft<LSFunction*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_left_int);
}
Compiler::value ArraySTD::fold_left_clo_int(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_left_clo_int = &LSArray<int>::ls_foldLeft<LSClosure*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_left_clo_int);
}

Compiler::value ArraySTD::fold_right_ptr(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_right_ptr = &LSArray<LSValue*>::ls_foldRight<LSFunction*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_right_ptr);
}
Compiler::value ArraySTD::fold_right_clo_ptr(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_right_clo_ptr = &LSArray<LSValue*>::ls_foldRight<LSClosure*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_right_clo_ptr);
}
Compiler::value ArraySTD::fold_right_real(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_right_real = &LSArray<double>::ls_foldRight<LSFunction*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_right_real);
}
Compiler::value ArraySTD::fold_right_clo_real(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_right_clo_real = &LSArray<double>::ls_foldRight<LSClosure*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_right_clo_real);
}
Compiler::value ArraySTD::fold_right_int(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_right_int = &LSArray<int>::ls_foldRight<LSFunction*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_right_int);
}
Compiler::value ArraySTD::fold_right_clo_int(Compiler& c, std::vector<Compiler::value> args) {
	auto fold_right_clo_int = &LSArray<int>::ls_foldRight<LSClosure*, LSValue*>;
	return c.insn_call(Type::any(), {args[0], args[1], c.insn_to_any(args[2])}, (void*) fold_right_clo_int);
}

Compiler::value ArraySTD::remove_element_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1])}, (void*) &LSArray<LSValue*>::ls_remove_element);
}
Compiler::value ArraySTD::remove_element_real(Compiler& c, std::vector<Compiler::value> args) {
	if (args[1].t.castable(Type::real())) {
		return c.insn_call(Type::boolean(), {args[0], c.to_real(args[1])}, (void*) &LSArray<double>::ls_remove_element);
	} else {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
}
Compiler::value ArraySTD::remove_element_int(Compiler& c, std::vector<Compiler::value> args) {
	if (args[1].t.castable(Type::integer())) {
		return c.insn_call(Type::boolean(), {args[0], c.to_int(args[1])}, (void*) &LSArray<int>::ls_remove_element);
	} else {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
}

Compiler::value ArraySTD::partition(Compiler& c, std::vector<Compiler::value> args) {
	auto array = args[0];
	auto function = args[1];
	auto array_true = c.new_array(array.t.element(), {});
	auto array_false = c.new_array(array.t.element(), {});
	c.insn_foreach(array, {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function.t.return_type(), {v}, function);
		c.insn_if(r, [&]() {
			c.insn_push_array(array_true, v);
		}, [&]() {
			c.insn_push_array(array_false, v);
		});
		c.insn_delete_temporary(r);
		return {};
	});
	return c.new_array(Type::array(array.t.element()), {array_true, array_false});
}

Compiler::value ArraySTD::map(Compiler& c, std::vector<Compiler::value> args) {
	auto array = args[0];
	auto function = args[1];
	auto result = c.new_array(function.t.return_type(), {});
	c.insn_foreach(array, {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto x = c.clone(v);
		c.insn_inc_refs(x);
		auto r = c.insn_call(function.t.return_type(), {x}, function);
		c.insn_push_array(result, r);
		c.insn_delete(x);
		return {};
	});
	return result;
}

}
