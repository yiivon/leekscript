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
		{Type::interval(), Type::integer(), Type::boolean(), (void*) &LSInterval::in_i, {}, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::interval(), {Type::interval()}, (void*) &ValueSTD::copy}
	});
	Type pred_fun_type_int = Type::fun(Type::boolean(), {Type::integer()});
	Type pred_clo_type_int = Type::closure(Type::boolean(), {Type::integer()});
	auto filter_fun = &LSInterval::ls_filter<LSFunction*>;
	auto filter_clo = &LSInterval::ls_filter<LSClosure*>;
	method("filter", {
		{Type::array(Type::integer()), {Type::interval(), pred_fun_type_int}, (void*) filter_fun, Method::NATIVE},
		{Type::array(Type::integer()), {Type::interval(), pred_clo_type_int}, (void*) filter_clo, Method::NATIVE}
	});
	method("sum", {
		{Type::long_(), {Type::interval()}, (void*) &LSInterval::ls_sum, Method::NATIVE},
	});
	method("product", {
		{Type::long_(), {Type::interval()}, (void*) &LSInterval::ls_product, Method::NATIVE},
	});
}

IntervalSTD::~IntervalSTD() {}

}
