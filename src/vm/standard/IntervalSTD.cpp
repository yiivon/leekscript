#include "IntervalSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSInterval.hpp"
#include "../value/LSClosure.hpp"

namespace ls {

IntervalSTD::IntervalSTD() : Module("Interval") {

	LSInterval::clazz = clazz;

	constructor_({
		{Type::tmp_interval(), {Type::integer(), Type::integer()}, (void*) &LSInterval::constructor, Method::NATIVE}
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::interval(), Type::integer(), Type::boolean(), (void*) &LSInterval::in_i, {}, Method::NATIVE}
	});
	operator_("~~", {
		{Type::const_interval(), Type::fun(Type::any(), {Type::integer()}), Type::array(Type::any()), (void*) &map},
		{Type::const_interval(), Type::fun(Type::real(), {Type::integer()}), Type::array(Type::real()), (void*) &map},
		{Type::const_interval(), Type::fun(Type::integer(), {Type::integer()}), Type::array(Type::integer()), (void*) &map},
		{Type::const_interval(), Type::fun(Type::boolean(), {Type::integer()}), Type::array(Type::boolean()), (void*) &map}
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
	method("map", {
		{Type::array(Type::any()), {Type::const_interval(), Type::fun(Type::any(), {Type::integer()})}, (void*) &map},
		{Type::array(Type::real()), {Type::const_interval(), Type::fun(Type::real(), {Type::integer()})}, (void*) &map},
		{Type::array(Type::integer()), {Type::const_interval(), Type::fun(Type::integer(), {Type::integer()})}, (void*) &map},
		{Type::array(Type::boolean()), {Type::const_interval(), Type::fun(Type::boolean(), {Type::integer()})}, (void*) &map}
	});
	method("sum", {
		{Type::long_(), {Type::interval()}, (void*) &LSInterval::ls_sum, Method::NATIVE},
	});
	method("product", {
		{Type::long_(), {Type::interval()}, (void*) &LSInterval::ls_product, Method::NATIVE},
	});

	/** Interval **/
	method("atv", {
		{Type::boolean(), {Type::interval(), Type::integer()}, (void*) &LSInterval::atv, Method::NATIVE}
	});
}

IntervalSTD::~IntervalSTD() {}

Compiler::value IntervalSTD::map(Compiler& c, std::vector<Compiler::value> args) {
	auto result = c.new_array(args[1].t.return_type(), {});
	c.insn_foreach(args[0], {}, "v", "", [&](Compiler::value value, Compiler::value key) -> Compiler::value {
		auto r = c.insn_call(args[1].t.return_type(), {value}, args[1]);
		if (r.t.is_bool()) {
			r = c.insn_convert(r, Type::any());
		}
		c.insn_push_array(result, r);
		return {};
	});
	return result;
}

}
