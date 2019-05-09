#include "ValueSTD.hpp"
#include "JsonSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include "../LSValue.hpp"

namespace ls {

ValueSTD::ValueSTD() : Module("Value") {

	LSValue::ValueClass = clazz;

	/*
	 * Static attributes
	 */
	static_field("unknown", Type::any(), ValueSTD::unknown);

	/*
	 * Attributes
	 */
	field("class", Type::clazz(), ValueSTD::attr_class);

	/*
	 * Operators
	 */
	operator_("is", {
		{Type::const_any(), Type::const_class(), Type::boolean(), (void*) &ValueSTD::op_instanceof}
	});
	operator_("==", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::eq, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_equals}
	});
	operator_("!=", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_not_equals}
	});
	operator_("<", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::lt, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_lt},
	});
	operator_("<=", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::le, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_le}
	});
	operator_(">", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::gt, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_gt}
	});
	operator_(">=", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::ge, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_ge}
	});
	operator_("and", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_and}
	});
	operator_("&&", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_and}
	});
	operator_("or", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_or}
	});
	operator_("||", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_or}
	});
	operator_("xor", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_xor}
	});
	operator_("+", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_add, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::op_add},
	});
	operator_("+=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_add_eq, {}, Method::NATIVE, true}
	});
	operator_("-", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_sub, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::op_sub},
	});
	operator_("-=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_sub_eq, {}, Method::NATIVE, true}
	});
	operator_("*", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_mul, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::op_mul}
	});
	operator_("*=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_mul_eq, {}, Method::NATIVE, true}
	});
	operator_("**", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_pow, {}, Method::NATIVE},
	});
	operator_("**=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_pow_eq, {}, Method::NATIVE, true}
	});
	operator_("/", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_div, {}, Method::NATIVE},
	});
	operator_("/=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_div_eq, {}, Method::NATIVE, true}
	});
	operator_("\\", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_int_div, {}, Method::NATIVE}
	});
	operator_("\\=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_int_div_eq, {}, Method::NATIVE, true}
	});
	operator_("%", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_mod, {}, Method::NATIVE}
	});
	operator_("%=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_mod_eq, {}, Method::NATIVE, true}
	});
	operator_("%%", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_double_mod, {}, Method::NATIVE},
	});
	operator_("%%=", {
		{Type::const_any(), Type::const_any(), Type::any(), (void*) &ValueSTD::ls_double_mod_eq, {}, Method::NATIVE, true}
	});
	operator_("&", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_and, {}, Method::NATIVE}
	});
	operator_("&=", {
		{Type::any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_and_eq, {}, Method::NATIVE, true}
	});
	operator_("|", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_or, {}, Method::NATIVE}
	});
	operator_("|=", {
		{Type::any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_or_eq, {}, Method::NATIVE, true}
	});
	operator_("^", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_xor, {}, Method::NATIVE}
	});
	operator_("^=", {
		{Type::any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_xor_eq, {}, Method::NATIVE, true}
	});
	operator_("<<", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::bit_shift_left}
	});
	operator_("<<=", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_shift_left_eq, {}, Method::NATIVE, true},
		{Type::integer(), Type::const_integer(), Type::integer(), (void*) &ValueSTD::bit_shift_left_eq, {}, false, true},
	});
	operator_(">>", {
		{Type::const_integer(), Type::const_integer(), Type::integer(), (void*) &ValueSTD::bit_shift_right}
	});
	operator_(">>=", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_shift_right_eq, {}, Method::NATIVE, true},
		{Type::const_integer(), Type::const_integer(), Type::integer(), (void*) &ValueSTD::bit_shift_right_eq, {}, false, true},
	});
	operator_(">>>", {
		{Type::const_integer(), Type::const_integer(), Type::integer(), (void*) &ValueSTD::bit_shift_uright}
	});
	operator_(">>>=", {
		{Type::const_any(), Type::const_any(), Type::integer(), (void*) &ValueSTD::ls_bit_shift_uright_eq, {}, Method::NATIVE, true},
		{Type::const_integer(), Type::const_integer(), Type::integer(), (void*) &ValueSTD::bit_shift_uright_eq, {}, false, true}
	});
	operator_("in", {
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::in, {}, Method::NATIVE},
		{Type::const_any(), Type::const_any(), Type::boolean(), (void*) &ValueSTD::op_in},
	});
	operator_("<=>", {
		{Type::any(), Type::any(), Type::any(), (void*) &ValueSTD::op_swap_ptr, {}, Method::NATIVE, true, true},
		{Type::integer(), Type::integer(), Type::integer(), (void*) &ValueSTD::op_swap_val, {}, false, true, true},
	});
	auto T = Type::template_("T");
	auto R = Type::template_("R");
	template_(T, R).
	operator_("~", {
		{T, Type::fun(R, {T}), R, (void*) &op_call},
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::copy}
	});
	method("string", {
		{Type::string(), {Type::const_any()}, (void*) &ValueSTD::to_string}
	});
	method("json", {
		{Type::tmp_string(), {Type::const_any()}, (void*) &LSValue::ls_json, Method::NATIVE},
		{Type::tmp_string(), {Type::const_any()}, (void*) &JsonSTD::encode},
	});
	method("typeID", {
		{Type::integer(), {Type::const_any()}, (void*) &ValueSTD::typeID}
	});
	method("move", {
		{Type::any(), {Type::const_any()}, (void*) &LSValue::move, Method::NATIVE}
	});
	method("move_inc", {
		{Type::any(), {Type::const_any()}, (void*) &LSValue::move_inc, Method::NATIVE}
	});
	method("ptr", {
		{Type::any(), {Type::const_any()}, (void*) &LSValue::move, Method::NATIVE}
	});
	method("absolute", {
		{Type::integer(), {Type::const_any()}, (void*) &ValueSTD::absolute}
	});
	method("clone", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::clone, Method::NATIVE}
	});
	method("delete", {
		{{}, {Type::const_any()}, (void*) &LSValue::free, Method::NATIVE}
	});
	method("not", {
		{Type::boolean(), {Type::const_any()}, (void*) &ValueSTD::ls_not, Method::NATIVE}
	});
	method("minus", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_minus, Method::NATIVE}
	});
	method("dec", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_dec, Method::NATIVE}
	});
	method("pre_dec", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_pre_dec, Method::NATIVE}
	});
	method("decl", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_decl, Method::NATIVE}
	});
	method("pre_decl", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_pre_decl, Method::NATIVE}
	});
	method("inc", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_inc, Method::NATIVE}
	});
	method("pre_inc", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_pre_inc, Method::NATIVE}
	});
	method("incl", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_incl, Method::NATIVE}
	});
	method("pre_incl", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_pre_incl, Method::NATIVE}
	});
	method("pre_tilde", {
		{Type::any(), {Type::const_any()}, (void*) &ValueSTD::ls_pre_tilde, Method::NATIVE}
	});
	method("attr", {
		{Type::any(), {Type::any(), Type::i8().pointer()}, (void*) &ValueSTD::attr, Method::NATIVE},
	});
	method("attrL", {
		{Type::any(), {Type::any(), Type::i8().pointer()}, (void*) &ValueSTD::attrL, Method::NATIVE},
	});
	method("int", {
		{Type::integer(), {Type::const_any()}, (void*) &ValueSTD::integer, Method::NATIVE}
	});
	method("real", {
		{Type::real(), {Type::const_any()}, (void*) &ValueSTD::real, Method::NATIVE}
	});
	method("long", {
		{Type::long_(), {Type::const_any()}, (void*) &ValueSTD::long_, Method::NATIVE}
	});
	method("range", {
		{Type::any(), {Type::const_any(), Type::integer(), Type::integer()}, (void*) &ValueSTD::range, Method::NATIVE}
	});
	method("at", {
		{Type::any(), {Type::const_any(), Type::const_any()}, (void*) &ValueSTD::at, Method::NATIVE}
	});
	method("atl", {
		{Type::any(), {Type::const_any(), Type::const_any()}, (void*) &ValueSTD::atl, Method::NATIVE}
	});
	method("in_i", {
		{Type::boolean(), {Type::const_any(), Type::integer()}, (void*) &ValueSTD::in_i, Method::NATIVE}
	});
	method("in", {
		{Type::boolean(), {Type::const_any(), Type::const_any()}, (void*) &ValueSTD::in, Method::NATIVE}
	});
	method("is_null", {
		{Type::boolean(), {Type::const_any()}, (void*) &ValueSTD::is_null, Method::NATIVE}
	});
	method("to_bool", {
		{Type::boolean(), {Type::const_any()}, (void*) &ValueSTD::to_bool, Method::NATIVE}
	});
	method("type", {
		{Type::integer(), {Type::const_any()}, (void*) &ValueSTD::type, Method::NATIVE}
	});
	method("delete_previous", {
		{{}, {Type::any()}, (void*) ValueSTD::delete_previous, Method::NATIVE}
	});
	method("get_int", {
		{Type::integer(), {Type::any()}, (void*) ValueSTD::get_int, Method::NATIVE}
	});
	method("get_class", {
		{Type::clazz(), {Type::any()}, (void*) ValueSTD::get_class, Method::NATIVE}
	});
}

/*
 * Static attributes
 */
Compiler::value ValueSTD::unknown(Compiler& c) {
	return c.insn_call(Type::any(), {c.new_integer(floor(1 + ((double) rand() / RAND_MAX) * 100))}, "Number.new");
}

/*
 * Attributes
 */
Compiler::value ValueSTD::attr_class(Compiler& c, Compiler::value a) {
	auto clazz = c.insn_class_of(a);
	c.insn_delete_temporary(a);
	return clazz;
}

/*
 * Operators
 */
Compiler::value ValueSTD::op_instanceof(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_eq(c.insn_class_of(args[0]), args[1]);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_equals(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_eq(args[0], args[1]);
}

Compiler::value ValueSTD::op_not_equals(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_ne(args[0], args[1]);
}

Compiler::value ValueSTD::op_lt(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0 or args[1].t.id() == 0) {
		auto ap = c.insn_to_any(args[0]);
		auto bp = c.insn_to_any(args[1]);
		auto res = c.insn_call(Type::boolean(), {ap, bp}, "Value.operator<");
		c.insn_delete_temporary(ap);
		c.insn_delete_temporary(bp);
		return res;
	} else {
		auto res = c.insn_lt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_le(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0	or args[1].t.id() == 0) {
		auto ap = c.insn_to_any(args[0]);
		auto bp = c.insn_to_any(args[1]);
		auto res = c.insn_call(Type::boolean(), {ap, bp}, "Value.operator<");
		c.insn_delete_temporary(ap);
		c.insn_delete_temporary(bp);
		return res;
	} else {
		auto res = c.insn_le(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_gt(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0	or args[1].t.id() == 0) {
		auto ap = c.insn_to_any(args[0]);
		auto bp = c.insn_to_any(args[1]);
		auto res = c.insn_call(Type::boolean(), {ap, bp}, "Value.operator>");
		c.insn_delete_temporary(ap);
		c.insn_delete_temporary(bp);
		return res;
	} else {
		auto res = c.insn_gt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_ge(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0 or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::boolean(), {c.insn_to_any(args[0]), c.insn_to_any(args[1])}, "Value.operator>=");
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	} else {
		auto res = c.insn_ge(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_and(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_and(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_or(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_or(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_xor(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_to_bool(args[0]);
	auto b = c.insn_to_bool(args[1]);
	auto r = c.insn_or(
		c.insn_and(a, c.insn_not_bool(b)),
		c.insn_and(b, c.insn_not_bool(a))
	);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_add(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_add(args[0], args[1]);
}
Compiler::value ValueSTD::op_sub(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_sub(args[0], args[1]);
}
Compiler::value ValueSTD::op_mul(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_mul(args[0], args[1]);
}
Compiler::value ValueSTD::op_div(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_div(args[0], args[1]);
}

int ValueSTD::ls_bit_and(LSValue* x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value & (int) b->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return res;
}

int ValueSTD::ls_bit_and_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value & (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

int ValueSTD::ls_bit_or(LSValue* x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
		(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value | (int) b->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return res;
}

int ValueSTD::ls_bit_or_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value | (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

int ValueSTD::ls_bit_xor(LSValue* x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
		(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value ^ (int) b->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return res;
}

int ValueSTD::ls_bit_xor_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value ^ (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

Compiler::value ValueSTD::bit_shift_left(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_shl(c.to_int(args[0]), c.to_int(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value ValueSTD::bit_shift_left_eq(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.pointed().is_primitive() && args[1].t.is_primitive()) {
		auto res = c.insn_shl(c.insn_load(args[0]), args[1]);
		c.insn_store(args[0], res);
		return res;
	} else {
		return c.insn_invoke(Type::integer(), {args[0], c.insn_to_any(args[1])}, "Value.operator<<=");
	}
}
int ValueSTD::ls_bit_shift_left_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value << (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}
Compiler::value ValueSTD::bit_shift_right(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_ashr(c.to_int(args[0]), c.to_int(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value ValueSTD::bit_shift_right_eq(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.pointed().is_primitive() && args[1].t.is_primitive()) {
		auto res = c.insn_ashr(c.insn_load(args[0]), args[1]);
		c.insn_store(args[0], res);
		return res;
	} else {
		return c.insn_invoke(Type::integer(), {args[0], c.insn_to_any(args[1])}, "Value.operator>>=");
	}
}
int ValueSTD::ls_bit_shift_right_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value >> (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

Compiler::value ValueSTD::bit_shift_uright(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_lshr(c.to_int(args[0]), c.to_int(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value ValueSTD::bit_shift_uright_eq(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.pointed().is_primitive() && args[1].t.is_primitive()) {
		auto res = c.insn_lshr(c.insn_load(args[0]), args[1]);
		c.insn_store(args[0], res);
		return res;
	} else {
		return c.insn_invoke(Type::integer(), {args[0], c.insn_to_any(args[1])}, "Value.operator>>>=");
	}
}
int ValueSTD::ls_bit_shift_uright_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (uint32_t) ((LSNumber*) a)->value >> (uint32_t) ((LSNumber*) b)->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

Compiler::value ValueSTD::op_in(Compiler& c, std::vector<Compiler::value> args) {
	if (args[1].t == Type::integer()) {
		return c.insn_invoke(Type::boolean(), args, "Value.in_i");
	} else {
		return c.insn_invoke(Type::boolean(), args, "Value.in");
	}
}
bool ValueSTD::in_i(LSValue* x, int k) {
	return x->in_i(k);
}
bool ValueSTD::in(LSValue* x, LSValue* y) {
	return x->in(y);
}

Compiler::value ValueSTD::op_swap_val(Compiler& c, std::vector<Compiler::value> args) {
	auto x_addr = args[0];
	auto y_addr = args[1];
	auto x = c.insn_load(x_addr);
	auto y = c.insn_load(y_addr);
	c.insn_store(x_addr, y);
	c.insn_store(y_addr, x);
	return y;
}

LSValue* ValueSTD::op_swap_ptr(LSValue** x, LSValue** y) {
	auto tmp = *x;
	*x = *y;
	*y = tmp;
	return *x;
}

Compiler::value ValueSTD::op_call(Compiler& c, std::vector<Compiler::value> args) {
	auto fun = args[1];
	return c.insn_call(fun.t.return_type(), {args[0]}, fun);
}

Compiler::value ValueSTD::copy(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.temporary) {
		return args[0];
	}
	return c.clone(args[0]);
}

Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.is_bool()) {
		return c.insn_call(Type::tmp_string(), args, "Boolean.to_string");
	} else if (args[0].t.is_integer()) {
		return c.insn_call(Type::tmp_string(), args, "Number.int_to_string");
	} else if (args[0].t.is_long()) {
		return c.insn_call(Type::tmp_string(), args, "Number.long_to_string");
	} else if (args[0].t.is_mpz_ptr()) {
		auto s = c.insn_call(Type::tmp_string(), args, "Number.mpz_to_string");
		c.insn_delete_temporary(args[0]);
		return s;
	} else if (args[0].t.is_real()) {
		return c.insn_call(Type::tmp_string(), args, "Number.real_to_string");
	} else {
		// Default type : pointer
		return c.insn_call(Type::tmp_string(), args, "Value.json");
	}
}

Compiler::value ValueSTD::typeID(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_typeof(args[0]);
}

int ValueSTD::absolute(LSValue* v) {
	return v->abso();
}
LSValue* ValueSTD::clone(LSValue* v) {
	return v->clone();
}
LSValue* ValueSTD::attr(LSValue* v, char* field) {
	return v->attr(field);
}
LSValue** ValueSTD::attrL(LSValue* v, char* field) {
	return v->attrL(field);
}
bool ValueSTD::ls_not(LSValue* x) {
	auto r = x->ls_not();
	LSValue::delete_temporary(x);
	return r;
}
LSValue* ValueSTD::ls_minus(LSValue* x) {
	return x->ls_minus();
}
LSValue* ValueSTD::ls_inc(LSValue* x) {
	return x->ls_inc();
}
LSValue* ValueSTD::ls_pre_inc(LSValue* x) {
	return x->ls_preinc();
}
LSValue* ValueSTD::ls_incl(LSValue** x) {
	return (*x)->ls_inc();
}
LSValue* ValueSTD::ls_pre_incl(LSValue** x) {
	return (*x)->ls_preinc();
}
LSValue* ValueSTD::ls_dec(LSValue* x) {
	return x->ls_dec();
}
LSValue* ValueSTD::ls_pre_dec(LSValue* x) {
	return x->ls_predec();
}
LSValue* ValueSTD::ls_decl(LSValue** x) {
	return (*x)->ls_dec();
}
LSValue* ValueSTD::ls_pre_decl(LSValue** x) {
	return (*x)->ls_predec();
}
LSValue* ValueSTD::ls_pre_tilde(LSValue* v) {
	return v->ls_tilde();
}
LSValue* ValueSTD::ls_add(LSValue* x, LSValue* y) {
	return x->add(y);
}
LSValue* ValueSTD::ls_add_eq(LSValue** x, LSValue* y) {
	return (*x)->add_eq(y);
}
LSValue* ValueSTD::ls_sub(LSValue* x, LSValue* y) {
	return x->sub(y);
}
LSValue* ValueSTD::ls_sub_eq(LSValue** x, LSValue* y) {
	return (*x)->sub_eq(y);
}
LSValue* ValueSTD::ls_mul(LSValue* x, LSValue* y) {
	return x->mul(y);
}
LSValue* ValueSTD::ls_mul_eq(LSValue** x, LSValue* y) {
	return (*x)->mul_eq(y);
}
LSValue* ValueSTD::ls_div(LSValue* x, LSValue* y) {
	return x->div(y);
}
LSValue* ValueSTD::ls_div_eq(LSValue** x, LSValue* y) {
	return (*x)->div_eq(y);
}
LSValue* ValueSTD::ls_int_div(LSValue* x, LSValue* y) {
	return x->int_div(y);
}
LSValue* ValueSTD::ls_int_div_eq(LSValue** x, LSValue* y) {
	return (*x)->int_div_eq(y);
}
LSValue* ValueSTD::ls_mod(LSValue* x, LSValue* y) {
	return x->mod(y);
}
LSValue* ValueSTD::ls_mod_eq(LSValue** x, LSValue* y) {
	return (*x)->mod_eq(y);
}
LSValue* ValueSTD::ls_double_mod(LSValue* x, LSValue* y) {
	return x->double_mod(y);
}
LSValue* ValueSTD::ls_double_mod_eq(LSValue** x, LSValue* y) {
	return (*x)->double_mod_eq(y);
}
LSValue* ValueSTD::ls_pow(LSValue* x, LSValue* y) {
	return x->pow(y);
}
LSValue* ValueSTD::ls_pow_eq(LSValue** x, LSValue* y) {
	return (*x)->pow_eq(y);
}

int ValueSTD::integer(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		return (int) number->value;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1 : 0;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
}
double ValueSTD::real(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		return number->value;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1.0 : 0.0;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
}
long ValueSTD::long_(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		return (long) number->value;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1l : 0l;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
}

LSValue* ValueSTD::range(LSValue* a, int start, int end) {
	return a->range(start, end);
}
LSValue* ValueSTD::at(LSValue* array, LSValue* key) {
	return array->at(key);
}
LSValue** ValueSTD::atl(LSValue* array, LSValue* key) {
	return array->atL(key);
}
int ValueSTD::type(LSValue* x) {
	return x->type;
}
bool ValueSTD::is_null(LSValue* x) {
	return x->type == LSValue::NULLL;
}
bool ValueSTD::to_bool(LSValue* x) {
	return x->to_bool();
}
bool ValueSTD::eq(LSValue* x, LSValue* y) {
	return *x == *y;
}
bool ValueSTD::lt(LSValue* x, LSValue* y) {
	return *x < *y;
}
bool ValueSTD::le(LSValue* x, LSValue* y) {
	return *x <= *y;
}
bool ValueSTD::gt(LSValue* x, LSValue* y) {
	return *x > *y;
}
bool ValueSTD::ge(LSValue* x, LSValue* y) {
	return *x >= *y;
}
void ValueSTD::delete_previous(LSValue* previous) {
	if (previous != nullptr) {
		LSValue::delete_ref(previous);
	}
}
int ValueSTD::get_int(LSNumber* x) {
	return (int) x->value;
}
LSValue* ValueSTD::get_class(LSValue* x) {
	return x->getClass();
}

}
