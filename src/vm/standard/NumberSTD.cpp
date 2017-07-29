#include "NumberSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSString.hpp"
#include "../../../lib/utf8.h"
#include "../../compiler/Compiler.hpp"
#include "../VM.hpp"

using namespace std;

namespace ls {

int mpz_log(__mpz_struct n) {
	int l = abs(n._mp_size) - 1;
	unsigned long s = n._mp_d[l];
	unsigned char r = 0;
	while (s >>= 1) r++;
	return 64 * l + r + 1;
}

NumberSTD::NumberSTD() : Module("Number") {

	LSNumber::clazz = clazz;

	static_field("pi", Type::REAL, [](Compiler& c) { return c.new_real(3.14159265358979323846); });
	static_field("e", Type::REAL, [](Compiler& c) { return c.new_real(M_E); });
	static_field("phi", Type::REAL, [](Compiler& c) { return c.new_real(1.61803398874989484820); });
	static_field("epsilon", Type::REAL, [](Compiler& c) { return c.new_real(std::numeric_limits<double>::epsilon()); });

	/*
	 * Operators
	 */
	operator_("+", {
		{Type::INTEGER, Type::POINTER, Type::POINTER, (void*) &NumberSTD::add_int_ptr, Method::NATIVE},
		{Type::MPZ, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::add_mpz_mpz},
		{Type::MPZ, Type::INTEGER, Type::MPZ_TMP, (void*) &NumberSTD::add_mpz_int},
		{Type::REAL, Type::REAL, Type::REAL, (void*) &NumberSTD::add_real_real},
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::add_real_real},
	});

	operator_("+=", {
		{Type::MPZ, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::add_eq_mpz_mpz}
	});

	operator_("-", {
		{Type::MPZ, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::sub_mpz_mpz},
		{Type::MPZ, Type::INTEGER, Type::MPZ_TMP, (void*) &NumberSTD::sub_mpz_int},
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::sub_real_real},
	});

	operator_("*", {
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::mul_real_real},
		{Type::CONST_REAL, Type::CONST_REAL, Type::REAL, (void*) &NumberSTD::mul_real_real},
		{Type::INTEGER, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::mul_int_mpz},
		{Type::MPZ, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::mul_mpz_mpz}
	});

	operator_("**", {
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::pow_real_real},
		{Type::CONST_REAL, Type::CONST_REAL, Type::REAL, (void*) &NumberSTD::pow_real_real},
		{Type::MPZ, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::pow_mpz_mpz},
		{Type::MPZ, Type::INTEGER, Type::MPZ_TMP, (void*) &NumberSTD::pow_mpz_int},
	});

	operator_("/", {
		{Type::NUMBER, Type::NUMBER, Type::REAL, (void*) &NumberSTD::div_val_val}
	});

	operator_("<", {
		{Type::NUMBER, Type::NUMBER, Type::BOOLEAN, (void*) &NumberSTD::lt},
		{Type::MPZ, Type::MPZ, Type::BOOLEAN, (void*) &NumberSTD::lt_mpz_mpz}
	});

	operator_("<=", {
		{Type::NUMBER, Type::NUMBER, Type::BOOLEAN, (void*) &NumberSTD::le}
	});

	operator_(">", {
		{Type::NUMBER, Type::NUMBER, Type::BOOLEAN, (void*) &NumberSTD::gt},
		{Type::INTEGER, Type::MPZ, Type::BOOLEAN, (void*) &NumberSTD::gt_int_mpz}
	});

	operator_(">=", {
		{Type::NUMBER, Type::NUMBER, Type::BOOLEAN, (void*) &NumberSTD::ge}
	});

	operator_("%", {
		{Type::MPZ, Type::MPZ, Type::MPZ_TMP, (void*) &NumberSTD::mod_mpz_mpz},
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::mod_real_real},
	});

	operator_("==", {
		{Type::MPZ, Type::MPZ, Type::BOOLEAN, (void*) &NumberSTD::eq_mpz_mpz},
		{Type::MPZ, Type::INTEGER, Type::BOOLEAN, (void*) &NumberSTD::eq_mpz_int}
	});

	Type tilde_fun_type_int = Type::FUNCTION_P;
	tilde_fun_type_int.setArgumentType(0, Type::INTEGER);
	tilde_fun_type_int.setReturnType(Type::POINTER);
	Type tilde_fun_type_real = Type::FUNCTION_P;
	tilde_fun_type_real.setArgumentType(0, Type::REAL);
	tilde_fun_type_real.setReturnType(Type::POINTER);
	operator_("~", {
		{Type::REAL, tilde_fun_type_real, Type::POINTER, (void*) &NumberSTD::tilde_real},
		{Type::INTEGER, tilde_fun_type_int, Type::POINTER, (void*) &NumberSTD::tilde_int}
	});

	operator_("&", {
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::bit_and}
	});
	operator_("&=", {
		{Type::INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::bit_and_eq}
	});
	operator_("|", {
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::bit_or}
	});
	operator_("|=", {
		{Type::INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::bit_or_eq}
	});
	operator_("^", {
		{Type::CONST_INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::bit_xor}
	});
	operator_("^=", {
		{Type::INTEGER, Type::CONST_INTEGER, Type::INTEGER, (void*) &NumberSTD::bit_xor_eq}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::REAL, {Type::CONST_REAL}, (void*) &ValueSTD::copy},
		{Type::LONG, {Type::CONST_LONG}, (void*) &ValueSTD::copy},
		{Type::INTEGER, {Type::CONST_INTEGER}, (void*) &ValueSTD::copy},
		{Type::MPZ, {Type::MPZ}, (void*) &ValueSTD::copy},
	});
	method("int", Method::Static, {
		{Type::INTEGER, {Type::UNKNOWN}, (void*) &NumberSTD::_int},
	});
	method("abs", {
		{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::abs_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::abs_number},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::abs_number},
	});
	method("acos", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::acos_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::acos_real},
	});
	method("asin", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::asin_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::asin_real},
	});
	method("atan", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::atan_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::atan_real},
	});
	method("atan2", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::atan2_ptr_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::atan2_ptr_real, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::POINTER}, (void*) &NumberSTD::atan2_real_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::atan2_real_real},
	});
	method("cbrt", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::cbrt_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::cbrt_real},
	});
	method("ceil", {
		{Type::INTEGER, {Type::NUMBER_P}, (void*) &NumberSTD::ceil_ptr, Method::NATIVE},
		//{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::ceil_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::ceil_real},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::ceil_int},
	});
	method("char", {
		{Type::POINTER, {Type::CONST_NUMBER_P}, (void*) &NumberSTD::char_ptr, Method::NATIVE},
		{Type::STRING, {Type::CONST_REAL}, (void*) &NumberSTD::char_real},
		{Type::STRING, {Type::CONST_INTEGER}, (void*) &NumberSTD::char_int},
		{Type::STRING, {Type::CONST_POINTER}, (void*) &NumberSTD::char_ptr, Method::NATIVE},
	});
	method("cos", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::cos_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::cos_real},
	});
	method("exp", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::exp_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::exp_real},
	});
	Type fold_fun_type = Type::FUNCTION_P;
	fold_fun_type.setArgumentType(0, Type::POINTER);
	fold_fun_type.setArgumentType(1, Type::INTEGER);
	fold_fun_type.setReturnType(Type::POINTER);
	auto fold_fun = &LSNumber::ls_fold<LSFunction*>;
	method("fold", {
		{Type::POINTER, {Type::POINTER, fold_fun_type, Type::POINTER}, (void*) fold_fun, Method::NATIVE}
	});
	method("floor", {
		{Type::INTEGER, {Type::NUMBER_P}, (void*) &NumberSTD::floor_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::floor_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::floor_real},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::floor_int},
	});
	method("hypot", {
		{Type::POINTER, {Type::REAL, Type::POINTER}, (void*) &NumberSTD::hypot_ptr_ptr},
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::hypot_ptr_ptr},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::hypot_real_real},
	});
	method("log", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::log_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::log_real},
	});
	method("log10", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::log10_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::log10_real},
	});
	method("max", {
		{Type::REAL, {Type::REAL, Type::POINTER}, (void*) &NumberSTD::max_float_ptr, Method::NATIVE},
		{Type::REAL, {Type::INTEGER, Type::POINTER}, (void*) &NumberSTD::max_int_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::NUMBER_P}, (void*) &NumberSTD::max_ptr_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::max_ptr_float, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::max_ptr_int, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::max_float_float},
		{Type::REAL, {Type::REAL, Type::INTEGER}, (void*) &NumberSTD::max_float_float},
		{Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &NumberSTD::max_float_float},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::max_float_float},

	});
	method("min", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::min_ptr_float, Method::NATIVE},
		{Type::REAL, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::min_ptr_int, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::min_float_float},
		{Type::REAL, {Type::REAL, Type::POINTER}, (void*) &NumberSTD::min_float_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::INTEGER}, (void*) &NumberSTD::min_float_float},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::min_float_float},
		{Type::REAL, {Type::INTEGER, Type::POINTER}, (void*) &NumberSTD::min_int_ptr, Method::NATIVE},
		{Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &NumberSTD::min_float_float},
	});
	method("pow", {
		{Type::REAL, {Type::POINTER, Type::NUMBER_P}, (void*) &NumberSTD::pow_ptr},
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::pow_ptr},
		{Type::LONG, {Type::LONG, Type::INTEGER}, (void*) &NumberSTD::pow_int},
		{Type::REAL, {Type::LONG, Type::LONG}, (void*) &NumberSTD::pow_int},
	});
	method("round", {
		{Type::INTEGER, {Type::NUMBER_P}, (void*) &NumberSTD::round_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::round_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::round_real},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::round_int}
	});
	method("rand", Method::Static, {
		{Type::REAL, {}, (void*) &NumberSTD::rand01, Method::NATIVE},
	});
	method("randInt", Method::Static, {
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::randInt, Method::NATIVE},
	});
	method("randFloat", Method::Static, {
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::randFloat, Method::NATIVE},
	});
	method("signum", {
		{Type::INTEGER, {Type::NUMBER_P}, (void*) &NumberSTD::signum, Method::NATIVE},
	});
	method("sin", {
		{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::sin_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::sin_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::sin_real},
	});
	method("sqrt", {
		{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::sqrt_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::sqrt_real},
		{Type::REAL, {Type::INTEGER}, (void*) &NumberSTD::sqrt_int, Method::NATIVE},
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::sqrt_ptr, Method::NATIVE},
		{Type::MPZ_TMP, {Type::MPZ}, (void*) NumberSTD::sqrt_mpz}
	});
	method("tan", {
		{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::tan_ptr, Method::NATIVE},
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::tan_ptr, Method::NATIVE},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::tan_real},
	});
	method("toDegrees", {
		{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::toDegrees, Method::NATIVE},
	});
	method("toRadians", {
		{Type::REAL, {Type::NUMBER_P}, (void*) &NumberSTD::toRadians, Method::NATIVE},
	});
	method("isInteger", {
		{Type::BOOLEAN, {Type::NUMBER_P}, (void*) &NumberSTD::isInteger, Method::NATIVE},
	});
	method("isPrime", {
		{Type::INTEGER, {Type::MPZ}, (void*) &NumberSTD::is_prime},
		{Type::BOOLEAN, {Type::LONG}, (void*) &NumberSTD::is_prime_long},
		{Type::BOOLEAN, {Type::INTEGER}, (void*) &NumberSTD::is_prime_int},
	});
}

Compiler::value NumberSTD::add_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_add(args[0], args[1]);
}

LSValue* NumberSTD::add_int_ptr(int a, LSValue* b) {
	return LSNumber::get(a)->add(b);
}

Compiler::value NumberSTD::add_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_add, "mpz_add");
	if (args[1].t.temporary && args[1] != r) {
		c.insn_delete_mpz(args[1]);
	}
	return r;
}

Compiler::value NumberSTD::add_mpz_int(Compiler& c, std::vector<Compiler::value> args) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		return c.new_mpz();
	}();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = args[1];
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_add_ui, "mpz_add_ui");
	return r;
}

Compiler::value NumberSTD::add_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a_addr, a_addr, b_addr}, &mpz_add);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::sub_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_sub(args[0], args[1]);
}

Compiler::value NumberSTD::sub_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_sub, "mpz_sub");
	if (args[1].t.temporary && args[1] != r) {
		c.insn_delete_mpz(args[1]);
	}
	return r;
}

Compiler::value NumberSTD::sub_mpz_int(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_address_of(args[0]);
	auto b = args[1];
	auto r = c.new_mpz();
	auto r_addr = c.insn_address_of(r);

	Compiler::label label_end;
	Compiler::label label_else;

	auto cond = c.insn_lt(b, c.new_integer(0));
	c.insn_branch_if_not(cond, &label_else);

	Compiler::value neg_b = {jit_insn_neg(c.F, b.v), Type::INTEGER};
	c.insn_call(Type::VOID, {r_addr, a, neg_b}, &mpz_add_ui, "mpz_add_ui");
	c.insn_branch(&label_end);

	c.insn_label(&label_else);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_sub_ui, "mpz_sub_ui");
	c.insn_label(&label_end);
	if (args[0].t.temporary) {
		c.insn_delete_mpz(args[0]);
	}
	return r;
}

Compiler::value NumberSTD::mul_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_mul(args[0], args[1]);
}

Compiler::value NumberSTD::mul_int_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.new_mpz();
	auto r_addr = c.insn_address_of(r);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, b, args[0]}, &mpz_mul_si, "mpz_mul_si");
	if (args[1].t.temporary) {
		c.insn_delete_mpz(args[1]);
	}
	return r;
}

Compiler::value NumberSTD::mul_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_mul, "mpz_mul");
	if (args[1].t.temporary && args[1] != r) {
		c.insn_delete_mpz(args[1]);
	}
	return r;
}

Compiler::value NumberSTD::div_val_val(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_div(args[0], args[1]);
}

Compiler::value NumberSTD::pow_real_real(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_pow(args[0], args[1]);
	if (args[0].t.raw_type == RawType::INTEGER && args[1].t.raw_type == RawType::INTEGER) {
		r = c.to_int(r);
	}
	return r;
}

Compiler::value NumberSTD::pow_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	auto p = c.insn_call(Type::LONG, {b}, &mpz_get_ui, "mpz_get_ui");
	c.insn_call(Type::VOID, {r_addr, a, p}, &mpz_pow_ui, "mpz_pow_ui");
	if (args[1].t.temporary && args[1] != r) {
		c.insn_delete_mpz(args[1]);
	}
	return r;
}

__mpz_struct pow_mpz_int_lambda(__mpz_struct a, int b) throw() {
	mpz_t res;
	mpz_init(res);
	mpz_pow_ui(res, &a, b);
	return *res;
}

Compiler::value NumberSTD::pow_mpz_int(Compiler& c, std::vector<Compiler::value> args) {
	// Check: mpz_log(a) * b <= 10000
	auto a_size = c.insn_call(Type::INTEGER, {args[0]}, (void*) &mpz_log, "mpz_log");
	auto r_size = c.insn_mul(a_size, args[1]);
	auto cond = c.insn_lt(r_size, c.new_integer(10000));
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if(c.F, cond.v, &label_end);

	c.insn_throw_object(vm::Exception::NUMBER_OVERFLOW);

	jit_insn_label(c.F, &label_end);

	// Ops: size of the theorical result
	c.inc_ops_jit(r_size);

	VM::inc_mpz_counter(c.F);
	return c.insn_call(Type::MPZ_TMP, args, &pow_mpz_int_lambda, "pow_mpz_int");
}

Compiler::value NumberSTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_lt(args[0], args[1]);
}

Compiler::value NumberSTD::lt_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, b_addr}, &mpz_cmp, "mpz_cmp");
	if (args[0].t.temporary) {
		c.insn_delete_mpz(args[0]);
	}
	if (args[1].t.temporary) {
		c.insn_delete_mpz(args[1]);
	}
	return c.insn_lt(res, c.new_integer(0));
}

Compiler::value NumberSTD::le(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_le(args[0], args[1]);
}

Compiler::value NumberSTD::gt(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_gt(args[0], args[1]);
}

Compiler::value NumberSTD::gt_int_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {b_addr, args[0]}, &_mpz_cmp_si, "_mpz_cmp_si");
	return c.insn_lt(res, c.new_integer(0));
}

Compiler::value NumberSTD::ge(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_ge(args[0], args[1]);
}

Compiler::value NumberSTD::mod_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_mod(args[0], args[1]);
}

Compiler::value NumberSTD::mod_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_mod, "mpz_mod");
	if (args[1].t.temporary && args[1] != r) {
		c.insn_delete_mpz(args[1]);
	}
	return r;
}

Compiler::value NumberSTD::eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, b_addr}, &mpz_cmp, "mpz_cmp");
	if (args[0].t.temporary) {
		c.insn_delete_mpz(args[0]);
	}
	if (args[1].t.temporary) {
		c.insn_delete_mpz(args[1]);
	}
	return c.insn_eq(res, c.new_integer(0));
}

Compiler::value NumberSTD::eq_mpz_int(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, args[1]}, &_mpz_cmp_si, "_mpz_cmp_si");
	if (args[0].t.temporary) {
		c.insn_delete_mpz(args[0]);
	}
	return c.insn_eq(res, c.new_integer(0));
}

Compiler::value NumberSTD::tilde_int(Compiler& c, std::vector<Compiler::value> args) {
	jit_type_t arg_types[] = {LS_POINTER, LS_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, arg_types, 2, 1);
	auto i = args[0].v;
	auto f = args[1].v;
	auto fun = jit_insn_load_relative(c.F, f, 24, LS_POINTER);
	jit_value_t jit_args[] = {f, i};
	Compiler::value r = {jit_insn_call_indirect(c.F, fun, sig, jit_args, 2, 0), Type::POINTER};
	// Double-free required here
	jit_type_free(sig);
	jit_type_free(sig);
	return r;
}

Compiler::value NumberSTD::tilde_real(Compiler& c, std::vector<Compiler::value> args) {
	jit_type_t arg_types[] = {LS_POINTER, LS_REAL};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, arg_types, 2, 1);
	auto r = args[0].v;
	auto f = args[1].v;
	auto fun = jit_insn_load_relative(c.F, f, 24, LS_POINTER);
	jit_value_t jit_args[] = {f, r};
	Compiler::value res = {jit_insn_call_indirect(c.F, fun, sig, jit_args, 2, 0), Type::POINTER};
	// Double-free required here
	jit_type_free(sig);
	jit_type_free(sig);
	return res;
}

Compiler::value NumberSTD::bit_and(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_bit_and(args[0], args[1]);
}

Compiler::value NumberSTD::bit_and_eq(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_bit_and(args[0], args[1]);
	c.insn_store(args[0], res);
	return res;
}

Compiler::value NumberSTD::bit_or(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_bit_or(args[0], args[1]);
}

Compiler::value NumberSTD::bit_or_eq(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_bit_or(args[0], args[1]);
	c.insn_store(args[0], res);
	return res;
}

Compiler::value NumberSTD::bit_xor(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_bit_xor(args[0], args[1]);
}

Compiler::value NumberSTD::bit_xor_eq(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_bit_xor(args[0], args[1]);
	c.insn_store(args[0], res);
	return res;
}

/*
 * Methods
 */

Compiler::value NumberSTD::_int(Compiler&, std::vector<Compiler::value> args) {
	return {args[0].v, Type::INTEGER};
}

Compiler::value NumberSTD::abs_ptr(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, {args[0]}, +[](LSNumber* n) {
		auto a = fabs(n->value);
		LSValue::delete_temporary(n);
		return a;
	});
}

Compiler::value NumberSTD::abs_number(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_abs(c.F, args[0].v), args[0].t};
}

double NumberSTD::acos_ptr(LSNumber* x) {
	double a = acos(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::acos_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_acos(c.F, args[0].v), Type::REAL};
}

double NumberSTD::asin_ptr(LSNumber* x) {
	double a = asin(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::asin_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_asin(c.F, args[0].v), Type::REAL};
}

double NumberSTD::atan_ptr(LSNumber* x) {
	double a = atan(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::atan_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_atan(c.F, args[0].v), Type::REAL};
}

double NumberSTD::atan2_ptr_ptr(LSNumber* y, LSNumber* x) {
	double a = atan2(y->value, x->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return a;
}

double NumberSTD::atan2_ptr_real(LSNumber* y, double x) {
	double a = atan2(y->value, x);
	LSValue::delete_temporary(y);
	return a;
}

double NumberSTD::atan2_real_ptr(double y, LSNumber* x) {
	double a = atan2(y, x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::atan2_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_atan2(c.F, args[0].v, args[1].v), Type::REAL};
}

LSString* NumberSTD::char_ptr(LSNumber* x) {
	unsigned int n = x->value;
	LSValue::delete_temporary(x);
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

Compiler::value NumberSTD::char_real(Compiler& c, vector<Compiler::value> args) {
	return c.insn_call(Type::STRING, args, +[](double x) {
		unsigned int n = x;
		char dest[5];
		u8_toutf8(dest, 5, &n, 1);
		return new LSString(dest);
	});
}

Compiler::value NumberSTD::char_int(Compiler& c, vector<Compiler::value> args) {
	return c.insn_call(Type::STRING, args, +[](int x) {
		unsigned int n = x;
		char dest[5];
		u8_toutf8(dest, 5, &n, 1);
		return new LSString(dest);
	});
}

double NumberSTD::exp_ptr(LSNumber* x) {
	double a = exp(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::exp_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_exp(c.F, args[0].v), Type::REAL};
}

int NumberSTD::floor_ptr(LSNumber* x) {
	int a = floor(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::floor_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_floor(c.F, args[0].v), Type::REAL};
}

Compiler::value NumberSTD::floor_int(Compiler&, std::vector<Compiler::value> args) {
	return args[0]; // Nothing to do
}

int NumberSTD::round_ptr(LSNumber* x) {
	int a = round(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::round_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_round(c.F, args[0].v), Type::REAL};
}

Compiler::value NumberSTD::round_int(Compiler&, std::vector<Compiler::value> args) {
	return args[0]; // Nothing to do
}

int NumberSTD::ceil_ptr(LSNumber* x) {
	int a = ceil(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::ceil_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_ceil(c.F, args[0].v), Type::REAL};
}

Compiler::value NumberSTD::ceil_int(Compiler&, std::vector<Compiler::value> args) {
	return args[0]; // Nothing to do
}

double NumberSTD::max_ptr_ptr(LSNumber* x, LSNumber* y) {
	double max = fmax(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::max_ptr_float(LSNumber* x, double y) {
	double max = fmax(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::max_ptr_int(LSNumber* x, int y) {
	double max = fmax(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::max_float_ptr(double x, LSNumber* y) {
	double max = fmax(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::max_int_ptr(int x, LSNumber* y) {
	double max = fmax(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
Compiler::value NumberSTD::max_float_float(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_max(c.F, args[0].v, args[1].v), Type::REAL};
}

double NumberSTD::min_ptr_ptr(LSNumber* x, LSNumber* y) {
	double min = fmin(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return min;
}
double NumberSTD::min_ptr_float(LSNumber* x, double y) {
	double min = fmin(x->value, y);
	LSValue::delete_temporary(x);
	return min;
}
double NumberSTD::min_ptr_int(LSNumber* x, int y) {
	double min = fmin(x->value, y);
	LSValue::delete_temporary(x);
	return min;
}
double NumberSTD::min_float_ptr(double x, LSNumber* y) {
	double min = fmin(x, y->value);
	LSValue::delete_temporary(y);
	return min;
}
double NumberSTD::min_int_ptr(int x, LSNumber* y) {
	double min = fmin(x, y->value);
	LSValue::delete_temporary(y);
	return min;
}
Compiler::value NumberSTD::min_float_float(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_min(c.F, args[0].v, args[1].v), Type::REAL};
}

double NumberSTD::cos_ptr(LSNumber* x) {
	double c = cos(x->value);
	LSValue::delete_temporary(x);
	return c;
}
Compiler::value NumberSTD::cos_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_cos(c.F, args[0].v), Type::REAL};
}

double NumberSTD::sin_ptr(LSNumber* x) {
	double s = sin(x->value);
	LSValue::delete_temporary(x);
	return s;
}
Compiler::value NumberSTD::sin_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_sin(c.F, args[0].v), Type::REAL};
}

double NumberSTD::tan_ptr(LSNumber* x) {
	double c = tan(x->value);
	LSValue::delete_temporary(x);
	return c;
}
Compiler::value NumberSTD::tan_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_tan(c.F, args[0].v), Type::REAL};
}

double NumberSTD::sqrt_ptr(LSNumber* x) {
	double s = sqrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}
double NumberSTD::sqrt_int(int x) {
	return sqrt(x);
}

Compiler::value NumberSTD::sqrt_mpz(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.new_mpz();
	auto r_addr = c.insn_address_of(r);
	auto a_addr = c.insn_address_of(args[0]);
	c.insn_call(Type::VOID, {r_addr, a_addr}, &mpz_sqrt, "mpz_sqrt");
	if (args[0].t.temporary) {
		c.insn_delete_mpz(args[0]);
	}
	return r;
}

Compiler::value NumberSTD::sqrt_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_sqrt(c.F, args[0].v), Type::REAL};
}

double NumberSTD::cbrt_ptr(LSNumber* x) {
	double s = cbrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}

Compiler::value NumberSTD::cbrt_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[] (double x) {
		return cbrt(x);
	});
}

Compiler::value NumberSTD::pow_int(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_pow(c.F, args[0].v, args[1].v), Type::LONG};
}

Compiler::value NumberSTD::is_prime(Compiler& c, std::vector<Compiler::value> args) {
	auto v_addr = c.insn_address_of(args[0]);
	auto reps = c.new_integer(15);
	auto res = c.insn_call(Type::INTEGER, {v_addr, reps}, &mpz_probab_prime_p, "mpz_probab_prime_p");
	if (args[0].t.temporary) {
		c.insn_delete_mpz(args[0]);
	}
	return res;
}

template<typename T>
int NumberSTD::is_prime_number(T n) {
	if (((!(n & 1)) and n != 2) or (n < 2) || (n % 3 == 0 and n != 3)) {
		return false;
	}
	for (T k = 1; 36 * k * k - 12 * k < n; ++k) {
		if ((n % (6 * k + 1) == 0) or (n % (6 * k - 1) == 0)) {
			return false;
		}
	}
	return true;
}
Compiler::value NumberSTD::is_prime_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::BOOLEAN, args, &NumberSTD::is_prime_number<int>);
}
Compiler::value NumberSTD::is_prime_long(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::BOOLEAN, args, &NumberSTD::is_prime_number<long>);
}

Compiler::value NumberSTD::hypot_ptr_ptr(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[](LSNumber* x, LSNumber* y) {
		auto r = hypot(x->value, y->value);
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return r;
	});
}
Compiler::value NumberSTD::hypot_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[](double x, double y) {
		return hypot(x, y);
	});
}

Compiler::value NumberSTD::log_ptr(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[](LSNumber* x) {
		auto res = log(x->value);
		LSValue::delete_temporary(x);
		return res;
	});
}
Compiler::value NumberSTD::log_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_log(c.F, args[0].v), Type::REAL};
}

Compiler::value NumberSTD::log10_ptr(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[](LSNumber* x) {
		auto res = log10(x->value);
		LSValue::delete_temporary(x);
		return res;
	});
}
Compiler::value NumberSTD::log10_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_log10(c.F, args[0].v), Type::REAL};
}

Compiler::value NumberSTD::pow_ptr(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[](LSNumber* x, LSNumber* y) {
		double r = pow(x->value, y->value);
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return r;
	});
}

double NumberSTD::rand01() {
	return (double) rand() / RAND_MAX;
}

double NumberSTD::randFloat(double min, double max) {
	return min + ((double) rand() / RAND_MAX) * (max - min);
}

int NumberSTD::randInt(int min, int max) {
	return min + floor(((double) rand() / RAND_MAX) * (max - min));
}

int NumberSTD::signum(LSNumber* x) {
	int s = 0;
	if (x->value > 0) s = 1;
	if (x->value < 0) s = -1;
	LSValue::delete_temporary(x);
	return s;
}

double NumberSTD::toDegrees(LSNumber* x) {
	double d = (x->value * 180) / M_PI;
	LSValue::delete_temporary(x);
	return d;
}

double NumberSTD::toRadians(LSNumber* x) {
	double r = (x->value * M_PI) / 180;
	LSValue::delete_temporary(x);
	return r;
}

bool NumberSTD::isInteger(LSNumber* x) {
	bool is = x->value == (int) x->value;
	LSValue::delete_temporary(x);
	return is;
}


}
