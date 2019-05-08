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
	 * Constructor
	 */
	constructor_({
		{Type::tmp_array(Type::integer()), {Type::integer()}, (void*) &LSArray<int>::constructor, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::integer()}, (void*) &LSArray<double>::constructor, Method::NATIVE},
		{Type::tmp_array(Type::any()), {Type::integer()}, (void*) &LSArray<LSValue*>::constructor, Method::NATIVE},
	});

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
		{Type::array(), Type::const_any(), Type::array(), (void*) &array_add_eq, {new WillStoreMutator()}, false, true},
	});

	auto ttE = Type::template_("E");
	auto ttR = Type::template_("R");
	template_(ttE, ttR).
	operator_("~~", {
		{Type::const_array(ttE), Type::fun(ttR, {ttE}), Type::tmp_array(ttR), (void*) &map},
	});

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
		{Type::boolean(), {Type::const_array(Type::any()), Type::const_any()}, (void*) &LSArray<LSValue*>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::const_array(Type::real()), Type::const_real()}, (void*) &LSArray<double>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::const_array(Type::integer()), Type::const_integer()}, (void*) &LSArray<int>::ls_contains, Method::NATIVE},
	});

	auto iter_ptr = &LSArray<LSValue*>::ls_iter<LSFunction*>;
	auto iE = Type::template_("E");
	template_(iE).
	method("iter", {
		{{}, {Type::const_array(iE), Type::fun({}, {iE})}, (void*) iter_ptr, Method::NATIVE},
		{{}, {Type::const_array(iE), Type::fun({}, {iE})}, (void*) &iter},
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

	auto map_fun = &LSArray<LSValue*>::ls_map<LSFunction*, LSValue*>;
	auto E = Type::template_("E");
	auto R = Type::template_("R");
	template_(E, R).
	method("map", {
		{Type::tmp_array(R), {Type::const_array(E), Type::fun(R, {E})}, (void*) map_fun, Method::NATIVE},
		{Type::tmp_array(R), {Type::const_array(E), Type::fun(R, {E})}, (void*) &map},
	});

	method("unique", {
		{Type::array(), {Type::array()}, (void*) &LSArray<LSValue*>::ls_unique, Method::NATIVE},
		{Type::array(Type::real()), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_unique, Method::NATIVE},
		{Type::array(Type::integer()), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_unique, Method::NATIVE},
	});

	auto sT = Type::template_("T");
	template_(sT).
	method("sort", {
		{Type::tmp_array(), {Type::array()}, (void*) &LSArray<LSValue*>::ls_sort, Method::NATIVE},
		{Type::tmp_array(Type::real()), {Type::array(Type::real())}, (void*) &LSArray<double>::ls_sort, Method::NATIVE},
		{Type::tmp_array(Type::integer()), {Type::array(Type::integer())}, (void*) &LSArray<int>::ls_sort, Method::NATIVE},
		{Type::tmp_array(sT), {Type::array(sT), Type::fun(Type::boolean(), {sT, sT})}, (void*) &sort}
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

	auto fT = Type::template_("T");
	template_(fT).
	method("first", {
		{fT, {Type::const_array(fT)}, (void*) &first}
	});

	auto lT = Type::template_("T");
	template_(lT).
	method("last", {
		{lT, {Type::const_array(lT)}, (void*) &last}
	});

	auto flT = Type::template_("T");
	auto flR = Type::template_("R");
	template_(flT, flR).
	method("foldLeft", {
		{flR, {Type::const_array(flT), Type::fun(flR, {flR, flT}), flR}, (void*) &fold_left},
	});

	auto frT = Type::template_("T");
	auto frR = Type::template_("R");
	template_(frT, frR).
	method("foldRight", {
		{frR, {Type::const_array(frT), Type::fun(frR, {frT, frR}), frR}, (void*) &fold_right},
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

	auto pT = Type::template_("T");
	template_(pT).
	method("push", {
		{Type::array(Type::any()), {Type::array(), Type::const_any()}, (void*) &LSArray<LSValue*>::ls_push, Method::NATIVE, {new WillStoreMutator()}},
		{Type::array(pT), {Type::array(Type::never()), pT}, (void*) &push, 0, {new WillStoreMutator()}},
		{Type::array(pT), {Type::array(pT), {}}, (void*) &push, 0, {new WillStoreMutator()}},
	});

	// void (LSArray<int>::*push_int)(int&&) = &LSArray<int>::push_back;
	method("vpush", {
		{{}, {Type::array(Type::integer()), Type::integer()}, (void*) &LSArray<int>::ls_push, Method::NATIVE},
		{{}, {Type::array(Type::real()), Type::real()}, (void*) &LSArray<double>::ls_push, Method::NATIVE},
		{{}, {Type::array(Type::any()), Type::any()}, (void*) &LSArray<LSValue*>::push_inc, Method::NATIVE},
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

	auto T = Type::template_("T");
	template_(T).
	method("fill", {
		{Type::array(T), {Type::array(), T}, (void*) &fill, false, {new WillStoreMutator()}},
		{Type::array(T), {Type::array(), T, Type::const_integer()}, (void*) &fill2, false, {new WillStoreMutator()}},
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
		{Type::boolean(), {Type::array(Type::real()), Type::const_real()}, (void*) &remove_element_real},
		{Type::boolean(), {Type::array(Type::integer()), Type::const_any()}, (void*) &remove_element_int},
		{Type::boolean(), {Type::array(Type::integer()), Type::integer()}, (void*) &remove_element_int},
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
		{Type::integer(), {Type::const_array(), Type::const_any(), Type::const_integer()}, (void*) &LSArray<LSValue*>::ls_search, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::real()), Type::const_real(), Type::const_integer()}, (void*) &LSArray<double>::ls_search, Method::NATIVE},
		{Type::integer(), {Type::const_array(Type::integer()), Type::const_integer(), Type::const_integer()}, (void*) &LSArray<int>::ls_search, Method::NATIVE},
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

	/** V1 **/
	method("count", {
		{Type::integer(), {Type::any()}, (void*) &LSArray<LSValue*>::ls_size, Method::NATIVE}
	});
	method("inArray", {
		{Type::boolean(), {Type::array(Type::any()), Type::any()}, (void*) &LSArray<LSValue*>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::real()), Type::real()}, (void*) &LSArray<double>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::array(Type::integer()), Type::integer()}, (void*) &LSArray<int>::ls_contains, Method::NATIVE}
	});

	/** Internal **/
	method("convert_key", {
		{Type::integer(), {Type::const_any(), Type::const_any()}, (void*) &ArraySTD::convert_key, Method::NATIVE}
	});
	method("in", {
		{Type::boolean(), {Type::const_array(Type::any()), Type::const_any()}, (void*) &LSArray<LSValue*>::in, Method::NATIVE},
		{Type::boolean(), {Type::const_array(Type::real()), Type::const_any()}, (void*) &LSArray<double>::in, Method::NATIVE},
		{Type::boolean(), {Type::const_array(Type::integer()), Type::integer()}, (void*) &LSArray<int>::in_i, Method::NATIVE},
	});
	method("isize", {
		{Type::integer(), {Type::array(Type::any())}, (void*) &LSArray<LSValue*>::int_size, Method::NATIVE},
		{Type::integer(), {Type::array(Type::real())}, (void*) &LSArray<double>::int_size, Method::NATIVE},
		{Type::integer(), {Type::array(Type::integer())}, (void*) &LSArray<int>::int_size, Method::NATIVE},
	});
	method("to_bool", {
		{Type::boolean(), {Type::array()}, (void*) &LSArray<int>::to_bool, Method::NATIVE}
	});
	auto sort_fun_int = &LSArray<int>::ls_sort_fun<LSFunction*>;
	auto sort_fun_real = &LSArray<double>::ls_sort_fun<LSFunction*>;
	auto sort_fun_any = &LSArray<LSValue*>::ls_sort_fun<LSFunction*>;
	method("sort_fun", {
		{Type::array(), {Type::array(), Type::fun({}, {})}, (void*) sort_fun_any, Method::NATIVE},
		{Type::array(), {Type::array(), Type::fun({}, {})}, (void*) sort_fun_real, Method::NATIVE},
		{Type::array(), {Type::array(), Type::fun({}, {})}, (void*) sort_fun_int, Method::NATIVE},
	});

}

Compiler::value ArraySTD::in(Compiler& c, std::vector<Compiler::value> args) {
	const auto& type = args[0].t.element().fold();
	auto f = [&]() {
		if (type.is_integer()) return "Array.in.2";
		if (type.is_real()) return "Array.in.1";
		return "Array.in";
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
	return c.insn_call(Type::array(args[0].t.element()), {args[0], c.insn_to_any(args[1])}, "Value.operator+");
}

Compiler::value ArraySTD::array_add_eq(Compiler& c, std::vector<Compiler::value> args) {
	args[1] = c.insn_to_any(args[1]);
	return c.insn_call(Type::any(), args, "Value.operator+=");
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

Compiler::value ArraySTD::fill(Compiler& c, std::vector<Compiler::value> args) {
	auto fun = [&]() {
		if (args[0].t.element().fold().is_integer()) return (void*) &LSArray<int>::ls_fill;
		if (args[0].t.element().fold().is_real()) return (void*) &LSArray<double>::ls_fill;
		return (void*) &LSArray<LSValue*>::ls_fill;
	}();
	return c.insn_call(args[0].t, {args[0], c.insn_convert(args[1], args[0].t.element().fold()), c.insn_array_size(args[0])}, fun);
}
Compiler::value ArraySTD::fill2(Compiler& c, std::vector<Compiler::value> args) {
	auto fun = [&]() {
		if (args[0].t.element().fold().is_integer()) return (void*) &LSArray<int>::ls_fill;
		if (args[0].t.element().fold().is_real()) return (void*) &LSArray<double>::ls_fill;
		return (void*) &LSArray<LSValue*>::ls_fill;
	}();
	return c.insn_call(args[0].t, {args[0], c.insn_convert(args[1], args[0].t.element().fold()), c.to_int(args[2])}, fun);
}

Compiler::value ArraySTD::fold_left(Compiler& c, std::vector<Compiler::value> args) {
	auto function = args[1];
	auto result = c.create_and_add_var("r", args[2].t);
	c.insn_store(result, c.insn_move_inc(args[2]));
	c.insn_foreach(args[0], {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function.t.return_type(), {c.insn_load(result), v}, function);
		c.insn_delete(c.insn_load(result));
		c.insn_store(result, c.insn_move_inc(r));
		return {};
	});
	return c.insn_load(result);
}

Compiler::value ArraySTD::fold_right(Compiler& c, std::vector<Compiler::value> args) {
	auto function = args[1];
	auto result = c.create_and_add_var("r", args[2].t);
	c.insn_store(result, c.insn_move(args[2]));
	c.insn_foreach(args[0], {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result, c.insn_call(function.t.return_type(), {v, c.insn_load(result)}, function));
		return {};
	}, true);
	return c.insn_load(result);
}

Compiler::value ArraySTD::iter(Compiler& c, std::vector<Compiler::value> args) {
	auto function = args[1];
	c.insn_foreach(args[0], {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		return c.insn_call(function.t.return_type(), {v}, function);
	});
	return {};
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

Compiler::value ArraySTD::map(Compiler& c, std::vector<Compiler::value> args, bool no_return) {
	auto array = args[0];
	auto function = args[1];
	auto result = no_return ? Compiler::value() : c.new_array(function.t.return_type(), {});
	c.insn_foreach(array, {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto x = c.clone(v);
		c.insn_inc_refs(x);
		auto r = c.insn_call(function.t.return_type(), {x}, function);
		if (no_return) {
			c.insn_delete_temporary(r);
		} else {
			c.insn_push_array(result, r.t.is_void() ? c.new_null() : r);
		}
		c.insn_delete(x);
		return {};
	});
	return no_return ? Compiler::value() : result;
}


Compiler::value ArraySTD::first(Compiler& c, std::vector<Compiler::value> args) {
	auto array = args[0];
	auto array_size = c.insn_array_size(array);
	c.insn_if(c.insn_ge(c.new_integer(0), array_size), [&]() {
		c.insn_delete_temporary(array);
		c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	});
	auto e = c.insn_move(c.insn_load(c.insn_array_at(array, c.new_integer(0))));
	c.insn_delete_temporary(array);
	return e;
}
Compiler::value ArraySTD::last(Compiler& c, std::vector<Compiler::value> args) {
	auto array = args[0];
	auto array_size = c.insn_array_size(array);
	c.insn_if(c.insn_ge(c.new_integer(0), array_size), [&]() {
		c.insn_delete_temporary(array);
		c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	});
	auto k = c.insn_sub(array_size, c.new_integer(1));
	auto e = c.insn_move(c.insn_load(c.insn_array_at(array, k)));
	c.insn_delete_temporary(array);
	return e;
}

Compiler::value ArraySTD::sort(Compiler& c, std::vector<Compiler::value> args) {
	const auto& array = args[0];
	const auto& fun = args[1];
	auto f = [&]() {
		if (args[0].t.element().fold().is_integer()) {
			return "Array.sort_fun.2";
		} else if (args[0].t.element().fold().is_real()) {
			return "Array.sort_fun.1";
		} else {
			return "Array.sort_fun";
		}
	}();
	return c.insn_call(array.t, {array, fun}, f);
}

Compiler::value ArraySTD::push(Compiler& c, std::vector<Compiler::value> args) {
	auto fun = [&]() {
		if (args[0].t.element().fold().is_integer()) return (void*) &LSArray<int>::ls_push;
		if (args[0].t.element().fold().is_real()) return (void*) &LSArray<double>::ls_push;
		return (void*) &LSArray<LSValue*>::ls_push;
	}();
	return c.insn_call(args[0].t, args, fun);
}

int ArraySTD::convert_key(LSValue* array, LSValue* key_pointer) {
	auto n = dynamic_cast<LSNumber*>(key_pointer);
	if (!n) {
		LSValue::delete_temporary(array);
		LSValue::delete_temporary(key_pointer);
		throw vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	}
	int key_int = n->value;
	LSValue::delete_temporary(key_pointer);
	return key_int;
}

}
