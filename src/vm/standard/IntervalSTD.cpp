#include "IntervalSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSInterval.hpp"
#include "../value/LSClosure.hpp"

namespace ls {

IntervalSTD::IntervalSTD() : Module("Interval") {

	LSInterval::clazz = clazz;

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::INTERVAL, Type::INTEGER, Type::BOOLEAN, (void*) &LSInterval::in_i, {}, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::INTERVAL, {Type::INTERVAL}, (void*) &ValueSTD::copy}
	});
	Type pred_fun_type_int = Type::fun(Type::BOOLEAN, {Type::INTEGER});
	Type pred_clo_type_int = Type::closure(Type::BOOLEAN, {Type::INTEGER});
	auto filter_fun = &LSInterval::ls_filter<LSFunction*>;
	auto filter_clo = &LSInterval::ls_filter<LSClosure*>;
	method("filter", {
		{Type::INT_ARRAY, {Type::INTERVAL, pred_fun_type_int}, (void*) filter_fun, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INTERVAL, pred_clo_type_int}, (void*) filter_clo, Method::NATIVE}
	});
	method("sum", {
		{Type::LONG, {Type::INTERVAL}, (void*) &LSInterval::ls_sum, Method::NATIVE},
	});
	method("product", {
		{Type::LONG, {Type::INTERVAL}, (void*) &LSInterval::ls_product, Method::NATIVE},
	});
}

IntervalSTD::~IntervalSTD() {}

}
