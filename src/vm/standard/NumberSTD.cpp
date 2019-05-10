#include "NumberSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSMpz.hpp"
#include "../value/LSString.hpp"
#include "../value/LSBoolean.hpp"
#include "../../../lib/utf8.h"
#include "../../compiler/Compiler.hpp"
#include "../VM.hpp"

namespace ls {

int mpz_log(__mpz_struct* n) {
	// std::cout << "mpz_log size " << n._mp_size << std::endl;
	// std::cout << "mpz_log alloc " << n._mp_alloc << std::endl;
	// std::cout << "mpz_log d " << n._mp_d << std::endl;
	int l = abs(n->_mp_size) - 1;
	unsigned long s = n->_mp_d[l];
	unsigned char r = 0;
	while (s >>= 1) r++;
	return 64 * l + r + 1;
}

NumberSTD::NumberSTD() : Module("Number") {

	LSNumber::clazz = clazz;

	static_field("pi", Type::real(), [](Compiler& c) { return c.new_real(3.14159265358979323846); });
	static_field("e", Type::real(), [](Compiler& c) { return c.new_real(M_E); });
	static_field("phi", Type::real(), [](Compiler& c) { return c.new_real(1.61803398874989484820); });
	static_field("epsilon", Type::real(), [](Compiler& c) { return c.new_real(std::numeric_limits<double>::epsilon()); });

	/*
	 * Constructors
	 */
	constructor_({
		{Type::any(), {Type::integer()}, (void*) &LSNumber::get},
		{Type::any(), {Type::tmp_mpz()}, (void*) &LSMpz::get_from_tmp},
		{Type::any(), {Type::mpz()}, (void*) &LSMpz::get_from_mpz},
	});

	/*
	 * Operators
	 */
	operator_("==", {
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::boolean(), NumberSTD::eq_mpz_mpz},
		{Type::mpz_ptr(), Type::integer(), Type::boolean(), NumberSTD::eq_mpz_int},
		{Type::integer(), Type::mpz_ptr(), Type::boolean(), NumberSTD::eq_int_mpz},
	});
	operator_("+", {
		// {Type::integer(), Type::any(), Type::any(), (void*) &NumberSTD::add_int_ptr},
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::add_mpz_mpz},
		{Type::mpz_ptr(), Type::integer(), Type::tmp_mpz_ptr(), NumberSTD::add_mpz_int},
		{Type::real(), Type::real(), Type::real(), NumberSTD::add_real_real},
		{Type::const_long(), Type::const_long(), Type::long_(), NumberSTD::add_real_real},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::add_real_real},
	});
	operator_("+=", {
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::add_eq_mpz_mpz, 0, {new ChangeTypeMutator()}, true},
		{Type::real(), Type::real(), Type::real(), NumberSTD::add_eq_real, 0, {new ChangeTypeMutator()}, true},
		{Type::integer(), Type::integer(), Type::integer(), NumberSTD::add_eq_real, 0, {new ChangeTypeMutator()}, true}
	});
	operator_("-", {
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::sub_mpz_mpz},
		{Type::mpz_ptr(), Type::integer(), Type::tmp_mpz_ptr(), NumberSTD::sub_mpz_int},
		{Type::const_real(), Type::const_real(), Type::const_real(), NumberSTD::sub_real_real},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::sub_real_real},
	});
	operator_("-=", {
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::sub_eq_mpz_mpz},
		{Type::real(), Type::real(), Type::real(), NumberSTD::sub_eq_real, 0, {}, true},
		{Type::integer(), Type::integer(), Type::integer(), NumberSTD::sub_eq_real, 0, {}, true}
	});
	operator_("*", {
		{Type::const_real(), Type::const_real(), Type::const_real(), NumberSTD::mul_real_real},
		{Type::const_long(), Type::const_long(), Type::long_(), NumberSTD::mul_real_real},
		{Type::const_integer(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::mul_int_mpz},
		{Type::mpz_ptr(), Type::integer(), Type::tmp_mpz_ptr(), NumberSTD::mul_mpz_int},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::mul_real_real},
		{Type::const_integer(), Type::const_string(), Type::string(), (void*) &NumberSTD::mul_int_string},
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::mul_mpz_mpz}
	});
	operator_("**", {
		{Type::const_real(), Type::const_real(), Type::real(), NumberSTD::pow_real_real},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::pow_real_real},
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::pow_mpz_mpz},
		{Type::mpz_ptr(), Type::integer(), Type::tmp_mpz_ptr(), NumberSTD::pow_mpz_int},
	});
	operator_("**=", {
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::pow_eq_mpz_mpz},
		{Type::real(), Type::real(), Type::real(), NumberSTD::pow_eq_real, 0, {}, true},
		{Type::integer(), Type::integer(), Type::integer(), NumberSTD::pow_eq_real, 0, {}, true}
	});
	operator_("*=", {
		{Type::mpz(), Type::mpz(), Type::tmp_mpz(), NumberSTD::mul_eq_mpz_mpz},
		{Type::real(), Type::real(), Type::real(), NumberSTD::mul_eq_real, 0, {}, true},
		{Type::integer(), Type::integer(), Type::integer(), NumberSTD::mul_eq_real, 0, {}, true}
	});
	operator_("/", {
		{Type::const_number(), Type::const_number(), Type::real(), NumberSTD::div_val_val}
	});
	operator_("/=", {
		{Type::mpz(), Type::mpz(), Type::tmp_mpz(), NumberSTD::div_eq_mpz_mpz},
		{Type::real(), Type::real(), Type::real(), NumberSTD::div_eq_real, 0, {}, true}
	});
	operator_("\\", {
		{Type::const_number(), Type::const_number(), Type::long_(), NumberSTD::int_div_val_val},
		{Type::integer(), Type::integer(), Type::integer(), NumberSTD::int_div_val_val},
	});
	operator_("\\=", {
		{Type::const_number(), Type::const_number(), Type::integer(), NumberSTD::int_div_eq_val_val, 0, {}, true}
	});
	operator_("<", {
		{Type::number(), Type::number(), Type::boolean(), NumberSTD::lt}
	});
	operator_("<=", {
		{Type::number(), Type::number(), Type::boolean(), NumberSTD::le}
	});
	operator_(">", {
		{Type::number(), Type::number(), Type::boolean(), NumberSTD::gt}
	});
	operator_(">=", {
		{Type::number(), Type::number(), Type::boolean(), NumberSTD::ge}
	});
	operator_("%", {
		{Type::const_real(), Type::const_real(), Type::real(), NumberSTD::mod},
		{Type::const_long(), Type::const_long(), Type::long_(), NumberSTD::mod},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::mod},
		{Type::mpz_ptr(), Type::mpz_ptr(), Type::tmp_mpz_ptr(), NumberSTD::mod_mpz_mpz},
	});
	operator_("%%", {
		{Type::const_real(), Type::const_real(), Type::real(), NumberSTD::double_mod},
		{Type::const_long(), Type::const_long(), Type::long_(), NumberSTD::double_mod},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::double_mod},
	});
	operator_("%%=", {
		{Type::const_real(), Type::const_real(), Type::real(), NumberSTD::double_mod_eq, 0, {}, true},
		{Type::const_long(), Type::const_long(), Type::long_(), NumberSTD::double_mod_eq, 0, {}, true},
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::double_mod_eq, 0, {}, true},
	});
	operator_("%=", {
		{Type::mpz(), Type::mpz(), Type::tmp_mpz(), NumberSTD::mod_eq_mpz_mpz, THROWS},
		{Type::real(), Type::real(), Type::real(), NumberSTD::mod_eq_real, THROWS, {}, true},
		{Type::integer(), Type::integer(), Type::integer(), NumberSTD::mod_eq_real, THROWS, {}, true},
		{Type::integer(), Type::long_(), Type::integer(), NumberSTD::mod_eq_real, THROWS, {}, true},
	});
	operator_("&", {
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::bit_and}
	});
	operator_("&=", {
		{Type::integer(), Type::const_integer(), Type::integer(), NumberSTD::bit_and_eq, 0, {}, true}
	});
	operator_("|", {
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::bit_or}
	});
	operator_("|=", {
		{Type::integer(), Type::const_integer(), Type::integer(), NumberSTD::bit_or_eq, 0, {}, true}
	});
	operator_("^", {
		{Type::const_integer(), Type::const_integer(), Type::integer(), NumberSTD::bit_xor}
	});
	operator_("^=", {
		{Type::integer(), Type::const_integer(), Type::integer(), NumberSTD::bit_xor_eq, 0, {}, true}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::real(), {Type::const_real()}, ValueSTD::copy},
		{Type::long_(), {Type::const_long()}, ValueSTD::copy},
		{Type::integer(), {Type::const_integer()}, ValueSTD::copy},
		{Type::mpz(), {Type::mpz()}, ValueSTD::copy},
	});
	method("int", Method::Static, {
		{Type::integer(), {Type::any()}, NumberSTD::_int},
		{Type::integer(), {Type::real()}, NumberSTD::_int}
	});
	method("long", Method::Static, {
		{Type::long_(), {Type::any()}, NumberSTD::_long},
	});
	method("abs", {
		{Type::real(), {Type::any()}, NumberSTD::abs},
		{Type::long_(), {Type::long_()}, NumberSTD::abs},
		{Type::integer(), {Type::integer()}, NumberSTD::abs},
	});
	int (*abs_int)(int) = std::abs;
	long (*abs_long)(long) = std::abs;
	double (*abs_real)(double) = std::fabs;
	method("m_abs", {
		{Type::integer(), {Type::integer()}, (void*) abs_int},
		{Type::long_(), {Type::long_()}, (void*) abs_long},
		{Type::real(), {Type::real()}, (void*) abs_real},
	});
	method("acos", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::acos_ptr},
		{Type::real(), {Type::real()}, NumberSTD::acos_real},
	});
	method("asin", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::asin_ptr},
		{Type::real(), {Type::real()}, NumberSTD::asin_real},
	});
	method("atan", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::atan_ptr},
		{Type::real(), {Type::real()}, NumberSTD::atan_real},
	});
	method("atan2", {
		{Type::any(), {Type::any(), Type::any()}, (void*) &NumberSTD::atan2_ptr_ptr},
		{Type::real(), {Type::real(), Type::any()}, NumberSTD::atan2},
		{Type::real(), {Type::real(), Type::real()}, NumberSTD::atan2},
	});
	double (*cbrtreal)(double) = std::cbrt;
	double (*cbrtint)(int) = std::cbrt;
	method("cbrt", {
		{Type::any(), {Type::any()}, (void*) &NumberSTD::cbrt_ptr},
		{Type::real(), {Type::real()}, (void*) cbrtreal},
		{Type::real(), {Type::integer()}, (void*) cbrtint},
	});
	method("ceil", {
		{Type::integer(), {Type::any()}, (void*) &NumberSTD::ceil_ptr},
		{Type::integer(), {Type::real()}, NumberSTD::ceil_real},
		{Type::integer(), {Type::integer()}, NumberSTD::ceil_int},
	});
	method("char", {
		{Type::tmp_string(), {Type::const_any()}, (void*) &NumberSTD::char_ptr},
		{Type::tmp_string(), {Type::const_real()}, (void*) &NumberSTD::char_real},
		{Type::tmp_string(), {Type::const_integer()}, (void*) &NumberSTD::char_int},
	});
	method("cos", {
		{Type::any(), {Type::any()}, (void*) &NumberSTD::cos_ptr},
		{Type::real(), {Type::real()}, NumberSTD::cos_real},
	});
	method("exp", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::exp_ptr},
		{Type::real(), {Type::real()}, NumberSTD::exp_real},
	});
	Type fold_fun_type = Type::fun(Type::any(), {Type::any(), Type::integer()});
	auto fold_fun = &LSNumber::ls_fold<LSFunction*>;
	method("fold", {
		{Type::any(), {Type::any(), fold_fun_type, Type::any()}, (void*) fold_fun},
		{Type::any(), {Type::any(), fold_fun_type, Type::any()}, fold}
	});
	method("floor", {
		{Type::integer(), {Type::any()}, (void*) &NumberSTD::floor_ptr},
		{Type::integer(), {Type::any()}, (void*) &NumberSTD::floor_ptr},
		{Type::integer(), {Type::real()}, NumberSTD::floor_real},
		{Type::integer(), {Type::integer()}, NumberSTD::floor_int},
	});
	method("hypot", {
		{Type::real(), {Type::any(), Type::any()}, NumberSTD::hypot_ptr_ptr},
		{Type::real(), {Type::integer(), Type::integer()}, (void*) std::hypot<int, int>},
		{Type::real(), {Type::real(), Type::real()}, (void*) std::hypot<double, double>},
	});
	method("log", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::log_ptr},
		{Type::real(), {Type::real()}, NumberSTD::log_real},
	});
	method("log10", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::log10_ptr},
		{Type::real(), {Type::long_()}, NumberSTD::log10_real},
		{Type::real(), {Type::real()}, NumberSTD::log10_real},
	});
	method("max", {
		{Type::any(), {Type::any(), Type::any()}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::real(), {Type::real(), Type::any()}, (void*) &NumberSTD::max_float_ptr},
		{Type::real(), {Type::integer(), Type::any()}, (void*) &NumberSTD::max_int_ptr},
		{Type::real(), {Type::any(), Type::real()}, (void*) &NumberSTD::max_ptr_float},
		{Type::real(), {Type::any(), Type::integer()}, (void*) &NumberSTD::max_ptr_int},
		{Type::real(), {Type::real(), Type::real()}, NumberSTD::max_float_float},
		{Type::real(), {Type::real(), Type::integer()}, NumberSTD::max_float_float},
		{Type::real(), {Type::integer(), Type::real()}, NumberSTD::max_float_float},
		{Type::integer(), {Type::integer(), Type::integer()}, NumberSTD::max_float_float}
	});
	method("min", {
		{Type::real(), {Type::any(), Type::any()}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::real(), {Type::any(), Type::real()}, (void*) &NumberSTD::min_ptr_float},
		{Type::real(), {Type::any(), Type::integer()}, (void*) &NumberSTD::min_ptr_int},
		{Type::real(), {Type::real(), Type::any()}, (void*) &NumberSTD::min_float_ptr},
		{Type::real(), {Type::real(), Type::real()}, NumberSTD::min_float_float},
		{Type::real(), {Type::real(), Type::integer()}, NumberSTD::min_float_float},
		{Type::real(), {Type::integer(), Type::any()}, (void*) NumberSTD::min_int_ptr},
		{Type::real(), {Type::integer(), Type::real()}, NumberSTD::min_float_float},
		{Type::integer(), {Type::integer(), Type::integer()}, NumberSTD::min_float_float},
	});
	method("pow", {
		{Type::real(), {Type::any(), Type::any()}, (void*) &NumberSTD::pow_ptr},
		{Type::long_(), {Type::long_(), Type::integer()}, NumberSTD::pow_int},
		{Type::real(), {Type::long_(), Type::long_()}, NumberSTD::pow_int},
	});
	method("round", {
		{Type::integer(), {Type::any()}, (void*) &NumberSTD::round_ptr},
		{Type::integer(), {Type::any()}, (void*) &NumberSTD::round_ptr},
		{Type::integer(), {Type::real()}, NumberSTD::round_real},
		{Type::integer(), {Type::integer()}, NumberSTD::round_int}
	});
	method("rand", Method::Static, {
		{Type::real(), {}, (void*) &NumberSTD::rand01},
	});
	method("randInt", Method::Static, {
		{Type::integer(), {Type::integer(), Type::integer()}, (void*) NumberSTD::rand_int},
	});
	method("randFloat", Method::Static, {
		{Type::real(), {Type::real(), Type::real()}, (void*) rand_real},
	});
	method("signum", {
		{Type::integer(), {Type::any()}, (void*) &NumberSTD::signum_ptr},
		{Type::integer(), {Type::number()}, NumberSTD::signum},
	});
	method("sin", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::sin_ptr},
		{Type::real(), {Type::any()}, (void*) &NumberSTD::sin_ptr},
		{Type::real(), {Type::real()}, NumberSTD::sin_real},
	});
	double (*sqrt_real)(double) = std::sqrt;
	method("sqrt", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::sqrt_ptr},
		{Type::tmp_mpz_ptr(), {Type::mpz_ptr()}, NumberSTD::sqrt_mpz},
		{Type::real(), {Type::real()}, (void*) sqrt_real},
		{Type::real(), {Type::integer()}, (void*) &std::sqrt<int>},
	});
	method("tan", {
		{Type::real(), {Type::any()}, (void*) &NumberSTD::tan_ptr},
		{Type::real(), {Type::any()}, (void*) &NumberSTD::tan_ptr},
		{Type::real(), {Type::real()}, NumberSTD::tan_real},
	});
	method("toDegrees", {
		{Type::any(), {Type::any()}, (void*) &NumberSTD::toDegrees_ptr},
		{Type::real(), {Type::any()}, NumberSTD::toDegrees},
	});
	method("toRadians", {
		{Type::any(), {Type::any()}, (void*) &NumberSTD::toRadians_ptr},
		{Type::real(), {Type::any()}, NumberSTD::toRadians},
	});
	method("isInteger", {
		{Type::any(), {Type::any()}, (void*) &NumberSTD::isInteger_ptr},
		{Type::boolean(), {Type::any()}, NumberSTD::isInteger},
	});
	method("isPrime", {
		{Type::boolean(), {Type::integer()}, (void*) &NumberSTD::is_prime_number<int>},
		{Type::integer(), {Type::mpz_ptr()}, NumberSTD::is_prime},
		{Type::boolean(), {Type::long_()}, (void*) &NumberSTD::is_prime_number<long>},
	});

	/** Internal **/
	method("powdd", {
		{Type::real(), {Type::real(), Type::real()}, (void*) &std::pow<double, double>}
	});
	method("powli", {
		{Type::real(), {Type::long_(), Type::integer()}, (void*) &std::pow<long, int>}
	});
	method("powii", {
		{Type::real(), {Type::integer(), Type::integer()}, (void*) &std::pow<int, int>}
	});
	method("mpz_init", {
		{{}, {Type::mpz().pointer()}, (void*) &mpz_init}
	});
	method("mpz_init_set", {
		{{}, {Type::mpz().pointer()}, (void*) &mpz_init_set}
	});
	method("mpz_init_str", {
		{{}, {Type::mpz().pointer(), Type::i8().pointer(), Type::integer()}, (void*) &mpz_init_set_str}
	});
	method("mpz_get_ui", {
		{{Type::long_()}, {Type::mpz().pointer()}, (void*) &mpz_get_ui}
	});
	method("mpz_get_si", {
		{{Type::long_()}, {Type::mpz().pointer()}, (void*) &mpz_get_si}
	});
	method("mpz_add", {
		{{}, {Type::mpz_ptr(), Type::mpz_ptr(), Type::mpz_ptr()}, (void*) &mpz_add}
	});
	method("mpz_add_ui", {
		{{}, {Type::mpz_ptr(), Type::long_(), Type::mpz_ptr()}, (void*) &mpz_add_ui}
	});
	method("mpz_sub", {
		{{}, {Type::mpz_ptr(), Type::mpz_ptr(), Type::mpz_ptr()}, (void*) &mpz_sub}
	});
	method("mpz_sub_ui", {
		{{}, {Type::mpz_ptr(), Type::long_(), Type::mpz_ptr()}, (void*) &mpz_sub_ui}
	});
	method("mpz_mul", {
		{{}, {Type::mpz().pointer(), Type::mpz().pointer(), Type::mpz().pointer()}, (void*) &mpz_mul}
	});
	method("mpz_mul_si", {
		{{}, {Type::mpz().pointer(), Type::long_(), Type::mpz().pointer()}, (void*) &mpz_mul_si}
	});
	method("mpz_pow_ui", {
		{{}, {Type::mpz().pointer(), Type::mpz().pointer(), Type::integer()}, (void*) &mpz_pow_ui}
	});
	method("mpz_mod", {
		{{}, {Type::mpz().pointer(), Type::mpz().pointer(), Type::mpz().pointer()}, (void*) &mpz_mod}
	});
	method("mpz_probab_prime_p", {
		{Type::integer(), {Type::mpz().pointer(), Type::integer()}, (void*) &mpz_probab_prime_p}
	});
	method("mpz_neg", {
		{{}, {Type::mpz().pointer(), Type::mpz().pointer()}, (void*) &mpz_neg}
	});
	method("mpz_log", {
		{{Type::integer()}, {Type::mpz().pointer()}, (void*) &mpz_log}
	});
	method("mpz_cmp", {
		{{Type::integer()}, {Type::mpz_ptr(), Type::mpz_ptr()}, (void*) &mpz_cmp}
	});
	method("_mpz_cmp_si", {
		{{Type::integer()}, {Type::mpz_ptr(), Type::long_()}, (void*) &_mpz_cmp_si}
	});
	method("mpz_sqrt", {
		{{}, {Type::mpz_ptr(), Type::mpz_ptr()}, (void*) &mpz_sqrt}
	});
	method("mpz_clear", {
		{{}, {Type::mpz().pointer()}, (void*) &mpz_clear}
	});
	method("int_to_string", {
		{Type::tmp_string(), {Type::integer()}, (void*) &NumberSTD::int_to_string}
	});
	method("long_to_string", {
		{Type::tmp_string(), {Type::long_()}, (void*) &NumberSTD::long_to_string}
	});
	method("real_to_string", {
		{Type::tmp_string(), {Type::real()}, (void*) &NumberSTD::real_to_string}
	});
	method("mpz_to_string", {
		{Type::tmp_string(), {Type::mpz_ptr()}, (void*) &NumberSTD::mpz_to_string}
	});
	double (*logreal)(double) = std::log;
	method("m_log", {
		{Type::real(), {Type::integer()}, (void*) &std::log<int>},
		{Type::real(), {Type::long_()}, (void*) &std::log<long>},
		{Type::real(), {Type::real()}, (void*) logreal},
	});
	double (*log10real)(double) = std::log10;
	method("m_log10", {
		{Type::real(), {Type::integer()}, (void*) &std::log10<int>},
		{Type::real(), {Type::long_()}, (void*) &std::log10<long>},
		{Type::real(), {Type::real()}, (void*) log10real},
	});
	double (*expreal)(double) = std::exp;
	method("m_exp", {
		{Type::integer(), {Type::integer()}, (void*) &std::exp<int>},
		{Type::real(), {Type::real()}, (void*) expreal},
	});
	double (*floorreal)(double) = std::floor;
	method("m_floor", {
		{Type::real(), {Type::real()}, (void*) floorreal},
	});
	double (*roundreal)(double) = std::round;
	method("m_round", {
		{Type::real(), {Type::real()}, (void*) roundreal},
	});
	double (*ceilreal)(double) = std::ceil;
	method("m_ceil", {
		{Type::real(), {Type::real()}, (void*) ceilreal},
	});
	method("m_max", {
		{Type::integer(), {Type::integer(), Type::integer()}, (void*) max<int>},
		{Type::long_(), {Type::long_(), Type::long_()}, (void*) max<long>},
		{Type::real(), {Type::real(), Type::real()}, (void*) max<double>},
	});
	method("m_min", {
		{Type::integer(), {Type::integer(), Type::integer()}, (void*) min<int>},
		{Type::long_(), {Type::long_(), Type::long_()}, (void*) min<long>},
		{Type::real(), {Type::real(), Type::real()}, (void*) min<double>},
	});
	double (*cosreal)(double) = std::cos;
	method("m_cos", {
		{Type::real(), {Type::integer()}, (void*) std::cos<int>},
		{Type::real(), {Type::long_()}, (void*) std::cos<long>},
		{Type::real(), {Type::real()}, (void*) cosreal},
	});
	double (*sinreal)(double) = std::sin;
	method("m_sin", {
		{Type::real(), {Type::integer()}, (void*) std::sin<int>},
		{Type::real(), {Type::long_()}, (void*) std::sin<long>},
		{Type::real(), {Type::real()}, (void*) sinreal},
	});
	double (*tanreal)(double) = std::tan;
	method("m_tan", {
		{Type::real(), {Type::integer()}, (void*) std::tan<int>},
		{Type::real(), {Type::long_()}, (void*) std::tan<long>},
		{Type::real(), {Type::real()}, (void*) tanreal},
	});
	double (*acosreal)(double) = std::acos;
	method("m_acos", {
		{Type::real(), {Type::integer()}, (void*) std::acos<int>},
		{Type::real(), {Type::long_()}, (void*) std::acos<long>},
		{Type::real(), {Type::real()}, (void*) acosreal},
	});
	double (*asinreal)(double) = std::asin;
	method("m_asin", {
		{Type::real(), {Type::integer()}, (void*) std::asin<int>},
		{Type::real(), {Type::long_()}, (void*) std::asin<long>},
		{Type::real(), {Type::real()}, (void*) asinreal},
	});
	double (*atanreal)(double) = std::atan;
	method("m_atan", {
		{Type::real(), {Type::integer()}, (void*) std::atan<int>},
		{Type::real(), {Type::long_()}, (void*) std::atan<long>},
		{Type::real(), {Type::real()}, (void*) atanreal},
	});
	double (*atan2real)(double, double) = std::atan2;
	method("m_atan2", {
		{Type::real(), {Type::integer(), Type::integer()}, (void*) std::atan2<int, int>},
		{Type::real(), {Type::long_(), Type::long_()}, (void*) std::atan2<long, long>},
		{Type::real(), {Type::real(), Type::real()}, (void*) atan2real},
	});
	method("m_isint", {
		{Type::boolean(), {Type::any()}, (void*) isint},
	});
}

Compiler::value NumberSTD::eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_eq(c.insn_call(Type::integer(), args, "Number.mpz_cmp"), c.new_integer(0));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value NumberSTD::eq_int_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_eq(c.insn_call(Type::integer(), {args[1], args[0]}, "Number._mpz_cmp_si"), c.new_integer(0));
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value NumberSTD::eq_mpz_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_eq(c.insn_call(Type::integer(), args, "Number._mpz_cmp_si"), c.new_integer(0));
	c.insn_delete_temporary(args[0]);
	return r;
}

Compiler::value NumberSTD::add_real_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_add(args[0], args[1]);
}

LSValue* NumberSTD::add_int_ptr(int a, LSValue* b) {
	return LSNumber::get(a)->add(b);
}

Compiler::value NumberSTD::add_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_add");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::add_mpz_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = args[0].t.temporary ? args[0] : c.new_mpz();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_add_ui");
	return r;
}

Compiler::value NumberSTD::add_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool no_return) {
	c.insn_call({}, {args[0], args[0], args[1]}, "Number.mpz_add");
	c.insn_delete_temporary(args[1]);
	return no_return ? Compiler::value() : c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::add_eq_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_add(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::sub_real_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_sub(args[0], args[1]);
}

Compiler::value NumberSTD::sub_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_sub");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::sub_mpz_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto a = args[0];
	auto b = args[1];

	Compiler::label label_then = c.insn_init_label("then");
	Compiler::label label_else = c.insn_init_label("else");
	Compiler::label label_end = c.insn_init_label("end");

	auto cond = c.insn_lt(b, c.new_integer(0));
	c.insn_if_new(cond, &label_then, &label_else);

	c.insn_label(&label_then);
	auto neg_b = c.insn_neg(b);
	auto r1 = c.new_mpz();
	c.insn_call({}, {r1, a, neg_b}, "Number.mpz_add_ui");
	c.insn_branch(&label_end);
	label_then.block = c.builder.GetInsertBlock();

	c.insn_label(&label_else);
	auto r2 = c.new_mpz();
	c.insn_call({}, {r2, a, b}, "Number.mpz_sub_ui");
	c.insn_branch(&label_end);
	label_else.block = c.builder.GetInsertBlock();
	
	c.insn_label(&label_end);
	auto PN = c.builder.CreatePHI(Type::mpz_ptr().llvm_type(c), 2);
	PN->addIncoming(r1.v, label_then.block);
	PN->addIncoming(r2.v, label_else.block);
	c.insn_delete_temporary(a);
	return {PN, Type::tmp_mpz_ptr()};
}

Compiler::value NumberSTD::sub_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call({}, {a_addr, a_addr, b_addr}, &mpz_sub);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::sub_eq_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_sub(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::mul_real_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_mul(args[0], args[1]);
}

Compiler::value NumberSTD::mul_int_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = args[1].t.temporary ? args[1] : c.new_mpz();
	c.insn_call({}, {r, args[1], args[0]}, "Number.mpz_mul_si");
	return r;
}

Compiler::value NumberSTD::mul_mpz_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = args[0].t.temporary ? args[0] : c.new_mpz();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_mul_si");
	return r;
}

LSValue* NumberSTD::mul_int_string(int a, LSString* b) {
	return b->mul(LSNumber::get(a));
}

Compiler::value NumberSTD::mul_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_mul");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::mul_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call({}, {a_addr, a_addr, b_addr}, &mpz_mul);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::mul_eq_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_mul(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::div_val_val(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_div(args[0], args[1]);
}

Compiler::value NumberSTD::pow_real_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_pow(args[0], args[1]);
	if (args[0].t.is_integer() && args[1].t.is_integer()) {
		r = c.to_int(r);
	}
	return r;
}

Compiler::value NumberSTD::div_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call({}, {a_addr, a_addr, b_addr}, &mpz_div);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::div_eq_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_div(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::int_div_val_val(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_int_div(args[0], args[1]);
}
Compiler::value NumberSTD::int_div_eq_val_val(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto r = c.insn_int_div(x, args[1]);
	c.insn_store(args[0], r);
	return r;
}

Compiler::value NumberSTD::pow_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto ui = c.insn_call(Type::long_(), {args[1]}, "Number.mpz_get_ui");
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call({}, {r, args[0], ui}, "Number.mpz_pow_ui");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::pow_mpz_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	// Check: mpz_log(a) * b <= 10000
	auto a_size = c.insn_call(Type::integer(), {args[0]}, "Number.mpz_log");
	auto r_size = c.insn_mul(a_size, args[1]);
	auto cond = c.insn_gt(r_size, c.new_integer(10000));
	auto label_then = c.insn_init_label("then");
	auto label_else = c.insn_init_label("else");
	auto label_end = c.insn_init_label("end");
	c.insn_if_new(cond, &label_then, &label_else);

	c.insn_label(&label_then);
	c.insn_delete_temporary(args[0]);
	c.insn_throw_object(vm::Exception::NUMBER_OVERFLOW);
	c.insn_branch(&label_end);

	c.insn_label(&label_else);
	c.insn_branch(&label_end);

	c.insn_label(&label_end);

	// Ops: size of the theorical result
	c.inc_ops_jit(r_size);

	auto r = args[0].t.temporary ? args[0] : c.new_mpz();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_pow_ui");
	return r;
}

Compiler::value NumberSTD::lt(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_lt(args[0], args[1]);
}
Compiler::value NumberSTD::le(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_le(args[0], args[1]);
}
Compiler::value NumberSTD::gt(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_gt(args[0], args[1]);
}
Compiler::value NumberSTD::ge(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_ge(args[0], args[1]);
}

Compiler::value NumberSTD::mod(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_mod(args[0], args[1]);
}
Compiler::value NumberSTD::mod_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = [&]() {
		if (args[0].t.temporary) return args[0];
		if (args[1].t.temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call({}, {r, args[0], args[1]}, "Number.mpz_mod");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::mod_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call({}, {a_addr, a_addr, b_addr}, &mpz_mod);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::mod_eq_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	// std::cout << "mod " << args[0].t << " " << args[1].t << std::endl;
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_mod(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::double_mod(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_double_mod(args[0], args[1]);
}
Compiler::value NumberSTD::double_mod_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto y = args[1];
	auto r = c.insn_mod(c.insn_add(c.insn_mod(x, y), y), y);
	c.insn_store(args[0], r);
	return r;
}

Compiler::value NumberSTD::bit_and(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_bit_and(args[0], args[1]);
}

Compiler::value NumberSTD::bit_and_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto res = c.insn_bit_and(c.insn_load(args[0]), args[1]);
	c.insn_store(args[0], res);
	return res;
}

Compiler::value NumberSTD::bit_or(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_bit_or(args[0], args[1]);
}

Compiler::value NumberSTD::bit_or_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto res = c.insn_bit_or(c.insn_load(args[0]), args[1]);
	c.insn_store(args[0], res);
	return res;
}

Compiler::value NumberSTD::bit_xor(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_bit_xor(args[0], args[1]);
}

Compiler::value NumberSTD::bit_xor_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto res = c.insn_bit_xor(c.insn_load(args[0]), args[1]);
	c.insn_store(args[0], res);
	return res;
}

/*
 * Methods
 */

Compiler::value NumberSTD::_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.to_int(args[0]);
}
Compiler::value NumberSTD::_long(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.to_long(args[0]);
}

Compiler::value NumberSTD::abs(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_abs(args[0]);
	c.insn_delete_temporary(args[0]);
	return r;
}

double NumberSTD::acos_ptr(LSNumber* x) {
	double a = acos(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::acos_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_acos(args[0]);
}

double NumberSTD::asin_ptr(LSNumber* x) {
	double a = asin(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::asin_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_asin(args[0]);
}

double NumberSTD::atan_ptr(LSNumber* x) {
	double a = atan(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::atan_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_atan(args[0]);
}

LSValue* NumberSTD::atan2_ptr_ptr(LSNumber* x, LSNumber* y) {
	auto r = LSNumber::get(std::atan2(x->value, y->value));
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
Compiler::value NumberSTD::atan2(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (!args[0].t.is_polymorphic() and !args[1].t.is_polymorphic()) {
		return c.insn_atan2(args[0], args[1]);
	} else {
		auto r = c.insn_call(Type::real(), {c.to_real(args[0]), c.to_real(args[1])}, "Number.m_atan2.2");
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return r;
	}
}

LSValue* NumberSTD::char_ptr(LSNumber* x) {
	unsigned int n = x->value;
	LSValue::delete_temporary(x);
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

LSValue* NumberSTD::char_real(double x) {
	unsigned int n = x;
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

LSValue* NumberSTD::char_int(int x) {
	unsigned int n = x;
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

double NumberSTD::exp_ptr(LSNumber* x) {
	double a = exp(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::exp_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_exp(args[0]);
}

int NumberSTD::floor_ptr(LSNumber* x) {
	int a = floor(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::floor_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_floor(args[0]);
}

Compiler::value NumberSTD::floor_int(Compiler&, std::vector<Compiler::value> args, bool) {
	return args[0]; // Nothing to do
}

int NumberSTD::round_ptr(LSNumber* x) {
	int a = round(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::round_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_round(args[0]);
}

Compiler::value NumberSTD::round_int(Compiler&, std::vector<Compiler::value> args, bool) {
	return args[0]; // Nothing to do
}

int NumberSTD::ceil_ptr(LSNumber* x) {
	int a = ceil(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::ceil_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_ceil(args[0]);
}

Compiler::value NumberSTD::ceil_int(Compiler&, std::vector<Compiler::value> args, bool) {
	return args[0]; // Nothing to do
}

LSValue* NumberSTD::max_ptr_ptr(LSNumber* x, LSNumber* y) {
	auto max = LSNumber::get(fmax(x->value, y->value));
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
Compiler::value NumberSTD::max_float_float(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_max(args[0], args[1]);
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
Compiler::value NumberSTD::min_float_float(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_min(args[0], args[1]);
}

LSValue* NumberSTD::cos_ptr(LSNumber* x) {
	LSValue* r = LSNumber::get(cos(x->value));
	LSValue::delete_temporary(x);
	return r;
}
Compiler::value NumberSTD::cos_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_cos(args[0]);
}

double NumberSTD::sin_ptr(LSNumber* x) {
	double s = sin(x->value);
	LSValue::delete_temporary(x);
	return s;
}
Compiler::value NumberSTD::sin_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_sin(args[0]);
}

double NumberSTD::tan_ptr(LSNumber* x) {
	double c = tan(x->value);
	LSValue::delete_temporary(x);
	return c;
}
Compiler::value NumberSTD::tan_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_tan(args[0]);
}

double NumberSTD::sqrt_ptr(LSNumber* x) {
	double s = sqrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}

Compiler::value NumberSTD::sqrt_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = args[0].t.temporary ? args[0] : c.new_mpz();
	c.insn_call({}, {r, args[0]}, "Number.mpz_sqrt");
	return r;
}

LSValue* NumberSTD::cbrt_ptr(LSNumber* x) {
	LSValue* r = LSNumber::get(cbrt(x->value));
	LSValue::delete_temporary(x);
	return r;
}

Compiler::value NumberSTD::pow_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_pow(args[0], args[1]);
}

Compiler::value NumberSTD::pow_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool) {
	c.insn_call({}, {args[0], args[0], args[1]}, "Number.mpz_pow_ui");
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::pow_eq_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_pow(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::is_prime(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto reps = c.new_integer(15);
	auto res = c.insn_call(Type::integer(), {args[0], reps}, "Number.mpz_probab_prime_p");
	c.insn_delete_temporary(args[0]);
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

Compiler::value NumberSTD::hypot_ptr_ptr(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_call(Type::real(), {c.to_real(args[0]), c.to_real(args[1])}, "Number.hypot.2");
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

double NumberSTD::log_ptr(LSNumber* x) {
	auto res = log(x->value);
	LSValue::delete_temporary(x);
	return res;
}
Compiler::value NumberSTD::log_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_log(args[0]);
}

double NumberSTD::log10_ptr(LSNumber* x) {
	auto res = log10(x->value);
	LSValue::delete_temporary(x);
	return res;
}
Compiler::value NumberSTD::log10_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_log10(args[0]);
}

double NumberSTD::pow_ptr(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

double NumberSTD::rand01() {
	return (double) rand() / RAND_MAX;
}

int NumberSTD::rand_int(int min, int max) {
	return min + floor(((double) rand() / RAND_MAX) * (max - min));
}
double NumberSTD::rand_real(double min, double max) {
	return min + ((double) rand() / RAND_MAX) * (max - min);
}

Compiler::value NumberSTD::signum(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto ap = c.insn_to_any(args[0]);
	auto r = c.insn_call(Type::integer(), {ap}, "Number.signum");
	c.insn_dec_refs(ap);
	return r;
}
int NumberSTD::signum_ptr(LSNumber* x) {
	int s = 0;
	if (x->value > 0) s = 1;
	if (x->value < 0) s = -1;
	LSValue::delete_temporary(x);
	return s;
}

double NumberSTD::toDegrees_ptr(LSNumber* x) {
	double d = (x->value * 180) / M_PI;
	LSValue::delete_temporary(x);
	return d;
}
Compiler::value NumberSTD::toDegrees(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::real(), {c.insn_to_any(args[0])}, "Number.toDegrees");
}
double NumberSTD::toRadians_ptr(LSNumber* x) {
	double r = (x->value * M_PI) / 180;
	LSValue::delete_temporary(x);
	return r;
}
Compiler::value NumberSTD::toRadians(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::real(), {c.insn_to_any(args[0])}, "Number.toRadians");
}

LSValue* NumberSTD::isInteger_ptr(LSNumber* x) {
	auto is = LSBoolean::get(x->value == (int) x->value);
	LSValue::delete_temporary(x);
	return is;
}
Compiler::value NumberSTD::isInteger(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto type = args[0].t.fold();
	if (type.is_integer() or type.is_long()) {
		return c.new_bool(true);
	} else if (type.is_primitive()) {
		return c.insn_eq(c.to_int(args[0]), args[0]);
	} else {
		return c.insn_call(Type::boolean(), args, "Number.m_isint");
	}
}

Compiler::value NumberSTD::fold(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::any(), {c.insn_to_any(args[0]), args[1], c.insn_to_any(args[2])}, "Number.fold");
}

LSValue* NumberSTD::int_to_string(int x) {
	return new LSString(std::to_string(x));
}
LSValue* NumberSTD::long_to_string(long x) {
	return new LSString(std::to_string(x));
}
LSValue* NumberSTD::real_to_string(double x) {
	return new LSString(LSNumber::print(x));
}
LSValue* NumberSTD::mpz_to_string(mpz_t x) {
	char buff[10000];
	mpz_get_str(buff, 10, x);
	return new LSString(buff);
}
bool NumberSTD::isint(LSNumber* x) {
	auto is = x->value == (int) x->value;
	LSValue::delete_temporary(x);
	return is;
}

}
