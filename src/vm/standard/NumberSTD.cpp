#include "NumberSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../../../lib/utf8.h"
#include "../../compiler/Compiler.hpp"

using namespace std;

namespace ls {

jit_value_t Number_e(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, M_E);
}
jit_value_t Number_phi(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, 1.61803398874989484820);
}
jit_value_t Number_pi(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, 3.14159265358979323846);
}
jit_value_t Number_epsilon(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, std::numeric_limits<double>::epsilon());
}

double number_asin(LSNumber* x) {
	double r = asin(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_atan(LSNumber* x) {
	double r = atan(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_atan2(LSNumber* x, LSNumber* y) {
	double r = atan2(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y->value;
	}
	return r;
}

double number_cbrt(LSNumber* x) {
	double r = cbrt(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_cos(LSNumber* x) {
	double r = cos(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_exp(LSNumber* x) {
	double r = exp(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_hypot(LSNumber* x, LSNumber* y) {
	double r = hypot(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y->value;
	}
	return r;
}

double number_log(LSNumber* x) {
	double r = log(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_log10(LSNumber* x) {
	double r = log10(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_pow(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

double number_rand() {
	return (double) rand() / RAND_MAX;
}

LSNumber* number_randFloat(LSNumber* min, LSNumber* max) {
	double r = min->value + ((double) rand() / RAND_MAX) * max->value;
	if (min->refs == 0) delete min;
	if (max->refs == 0) {
		max->value = r;
		return max;
	}
	return LSNumber::get(r);
}

int number_randInt(int min, int max) {
	return floor(min + ((double) rand() / RAND_MAX) * (max - min));
}

LSNumber* number_signum(LSNumber* x) {
	double r = 0;
	if (x->value > 0) r = 1;
	if (x->value < 0) r = -1;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_sin(LSNumber* x) {
	double r = sin(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_tan(LSNumber* x) {
	double r = tan(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_toDegrees(LSNumber* x) {
	double r = (x->value * 180) / M_PI;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_toRadians(LSNumber* x) {
	double r = (x->value * M_PI) / 180;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

bool number_isInteger(LSNumber* x) {
	bool is = x->value == (int) x->value;
	if (x->refs == 0) delete x;
	return is;
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
NumberSTD::NumberSTD() : Module("Number") {

	operator_("+", {
		{Type::REAL, Type::REAL, Type::REAL, (void*) &NumberSTD::add_real_real, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, Type::INTEGER, (void*) &NumberSTD::add_real_real, Method::NATIVE},
	});

	static_field("pi", Type::REAL, (void*) &Number_pi);
	static_field("e", Type::REAL, (void*) &Number_e);
	static_field("phi", Type::REAL, (void*) &Number_phi);
	static_field("epsilon", Type::REAL, (void*) &Number_epsilon);

	method("abs", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::abs_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::abs_real, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {}, (void*) &NumberSTD::abs_real, Method::NATIVE}
	});
	method("acos", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::acos_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::acos_real, Method::NATIVE},
	});
	method("asin", Type::REAL, Type::REAL, {}, (void*) &number_asin);
	method("atan", Type::REAL, Type::REAL, {}, (void*) &number_atan);
	method("cbrt", Type::REAL, Type::REAL, {}, (void*) &number_cbrt);

	method("ceil", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::ceil_ptr}
	});

	method("char", {
		{Type::NUMBER, Type::POINTER, {}, (void*) &NumberSTD::char_ptr},
		{Type::REAL, Type::STRING, {}, (void*) &NumberSTD::char_real, Method::NATIVE},
		{Type::INTEGER, Type::STRING, {}, (void*) &NumberSTD::char_int, Method::NATIVE}
	});

	method("cos", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::cos_ptr}
	});
	method("exp", Type::NUMBER, Type::REAL, {}, (void*) &number_exp);

	method("floor", {
		{Type::NUMBER, Type::INTEGER, {}, (void*) &NumberSTD::floor_ptr},
		{Type::REAL, Type::INTEGER, {}, (void*) &NumberSTD::floor_ptr}
	});

	method("hypot", Type::NUMBER, Type::REAL, {Type::NUMBER}, (void*) &number_hypot);
	method("log", Type::NUMBER, Type::REAL, {}, (void*) &number_log);
	method("log10", Type::NUMBER, Type::REAL, {}, (void*) &number_log10);

	method("max", {
		{Type::POINTER, Type::REAL, {Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::REAL, Type::REAL, {Type::REAL}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
	});

	method("min", {
		{Type::POINTER, Type::REAL, {Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::REAL, Type::REAL, {Type::REAL}, (void*) &NumberSTD::min_float_float},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::min_float_float},
	});

	method("pow", {
		{Type::NUMBER, Type::REAL, {Type::NUMBER}, (void*) &number_pow},
		{Type::INTEGER, Type::LONG, {Type::INTEGER}, (void*) &NumberSTD::pow_int, Method::NATIVE}
	});

	method("round", {
		{Type::NUMBER, Type::INTEGER, {}, (void*) &NumberSTD::round_ptr}
	});

	method("signum", Type::NUMBER, Type::INTEGER, {}, (void*) &number_signum);

	method("sin", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::sin_ptr}
	});

	method("sqrt", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::sqrt_ptr}
	});
	method("tan", Type::NUMBER, Type::REAL, {}, (void*) &number_tan);
	method("toDegrees", Type::NUMBER, Type::REAL, {}, (void*) &number_toDegrees);
	method("toRadians", Type::NUMBER, Type::REAL, {}, (void*) &number_toRadians);
	method("isInteger", Type::NUMBER, Type::BOOLEAN, {}, (void*) &number_isInteger);

	/*
	 * Static methods
	 */
	static_method("abs", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::abs_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::abs_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::abs_real, Method::NATIVE}
	});
	static_method("acos", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::acos_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::acos_real, Method::NATIVE},
	});
	static_method("asin", Type::REAL, {Type::NUMBER}, (void*) &number_asin);
	static_method("atan", Type::REAL, {Type::NUMBER}, (void*) &number_atan);
	static_method("atan2", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_atan2);
	static_method("cbrt", Type::REAL, {Type::NUMBER}, (void*) &number_cbrt);

	static_method("ceil", {
		{Type::REAL, {Type::NUMBER}, (void*) &NumberSTD::ceil_ptr},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::ceil_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::ceil_int, Method::NATIVE}
	});

	static_method("char", {
		{Type::STRING, {Type::POINTER}, (void*) &NumberSTD::char_ptr},
		{Type::STRING, {Type::REAL}, (void*) &NumberSTD::char_real, Method::NATIVE},
		{Type::STRING, {Type::INTEGER}, (void*) &NumberSTD::char_int, Method::NATIVE}
	});

	static_method("cos", {
		{Type::REAL, {Type::NUMBER}, (void*) &NumberSTD::cos_ptr}
	});

	static_method("exp", Type::REAL, {Type::NUMBER}, (void*) &number_exp);

	static_method("floor", {
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::floor_ptr},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::floor_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::floor_int, Method::NATIVE},
	});

	static_method("hypot", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_hypot);
	static_method("log", Type::REAL, {Type::NUMBER}, (void*) &number_log);
	static_method("log10", Type::REAL, {Type::NUMBER}, (void*) &number_log10);

	static_method("max", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::max_ptr_float},
		{Type::REAL, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::max_ptr_int},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE}
	});

	static_method("min", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::min_ptr_float},
		{Type::REAL, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::min_ptr_int},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::min_float_float},
		{Type::REAL, {Type::REAL, Type::INTEGER}, (void*) &NumberSTD::min_float_float},
		{Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &NumberSTD::min_float_float},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::min_float_float}
	});

	static_method("pow", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_pow);
	static_method("rand", Type::REAL, {}, (void*) &number_rand);
	static_method("randFloat", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_randFloat);
	static_method("randInt", Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &number_randInt);

	static_method("round", {
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::round_ptr},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::round_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::round_int, Method::NATIVE}
	});

	static_method("signum", Type::INTEGER, {Type::NUMBER}, (void*) &number_signum);

	static_method("sin", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::sin_ptr}
	});

	static_method("sqrt", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::sqrt_ptr}
	});

	static_method("tan", Type::REAL, {Type::NUMBER}, (void*) &number_tan);
	static_method("toDegrees", Type::REAL, {Type::NUMBER}, (void*) &number_toDegrees);
	static_method("toRadians", Type::REAL, {Type::NUMBER}, (void*) &number_toRadians);
	static_method("isInteger", Type::BOOLEAN, {Type::NUMBER}, (void*) &number_isInteger);


}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

jit_value_t NumberSTD::add_real_real(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_add(c.F, args[0], args[1]);
}

double NumberSTD::abs_ptr(LSNumber* x) {
	// TODO check args
	// VM::check_arg(args, {LS_NUMBER})
	// Number.abs(["hello", 12][0])
	// ==>  Execution error Number.abs(number x) : first parameter 'x' is not a number but a string.
	double a = fabs(x->value);
	LSValue::delete_temporary(x);
	return a;
}
jit_value_t NumberSTD::abs_real(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_abs(c.F, args[0]);
}

double NumberSTD::acos_ptr(LSNumber* x) {
	double a = acos(x->value);
	LSValue::delete_temporary(x);
	return a;
}
jit_value_t NumberSTD::acos_real(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_acos(c.F, args[0]);
}

LSString* NumberSTD::char_ptr(LSNumber* x) {
	unsigned int n = x->value;
	LSValue::delete_temporary(x);
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}
jit_value_t NumberSTD::char_real(Compiler& c, vector<jit_value_t> args) {
	return VM::call(c, LS_STRING, {LS_REAL}, args, +[](double x) {
		unsigned int n = x;
		char dest[5];
		u8_toutf8(dest, 5, &n, 1);
		return new LSString(dest);
	});
}
jit_value_t NumberSTD::char_int(Compiler& c, vector<jit_value_t> args) {
	return VM::call(c, LS_STRING, {LS_INTEGER}, args, +[](int x) {
		unsigned int n = x;
		char dest[5];
		u8_toutf8(dest, 5, &n, 1);
		return new LSString(dest);
	});
}

int NumberSTD::floor_ptr(LSNumber* x) {
	int a = floor(x->value);
	LSValue::delete_temporary(x);
	return a;
}
jit_value_t NumberSTD::floor_real(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_floor(c.F, args[0]);
}
jit_value_t NumberSTD::floor_int(Compiler&, std::vector<jit_value_t> args) {
	return args[0]; // Nothing to do
}

int NumberSTD::round_ptr(LSNumber* x) {
	int a = round(x->value);
	LSValue::delete_temporary(x);
	return a;
}
jit_value_t NumberSTD::round_real(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_round(c.F, args[0]);
}
jit_value_t NumberSTD::round_int(Compiler&, std::vector<jit_value_t> args) {
	return args[0]; // Nothing to do
}

int NumberSTD::ceil_ptr(LSNumber* x) {
	int a = ceil(x->value);
	LSValue::delete_temporary(x);
	return a;
}
jit_value_t NumberSTD::ceil_real(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_ceil(c.F, args[0]);
}
jit_value_t NumberSTD::ceil_int(Compiler&, std::vector<jit_value_t> args) {
	return args[0]; // Nothing to do
}

double NumberSTD::max_ptr_ptr(LSNumber* x, LSNumber* y) {
	int max = fmax(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::max_ptr_float(LSNumber* x, double y) {
	int max = fmax(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::max_ptr_int(LSNumber* x, int y) {
	int max = fmax(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::max_float_ptr(double x, LSNumber* y) {
	int max = fmax(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::max_int_ptr(int x, LSNumber* y) {
	int max = fmax(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
jit_value_t NumberSTD::max_float_float(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_max(c.F, args[0], args[1]);
}

double NumberSTD::min_ptr_ptr(LSNumber* x, LSNumber* y) {
	int max = fmin(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::min_ptr_float(LSNumber* x, double y) {
	int max = fmin(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::min_ptr_int(LSNumber* x, int y) {
	int max = fmin(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::min_float_ptr(double x, LSNumber* y) {
	int max = fmin(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::min_int_ptr(int x, LSNumber* y) {
	int max = fmin(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
jit_value_t NumberSTD::min_float_float(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_min(c.F, args[0], args[1]);
}

double NumberSTD::cos_ptr(LSNumber* x) {
	double c = cos(x->value);
	LSValue::delete_temporary(x);
	return c;
}

double NumberSTD::sin_ptr(LSNumber* x) {
	double s = sin(x->value);
	LSValue::delete_temporary(x);
	return s;
}

double NumberSTD::sqrt_ptr(LSNumber* x) {
	double s = sqrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}

jit_value_t NumberSTD::pow_int(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_pow(c.F, args[0], args[1]);
}

}
