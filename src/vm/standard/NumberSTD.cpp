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

LSNumber* number_acos(LSNumber* x) {
	double r = acos(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_asin(LSNumber* x) {
	double r = asin(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_atan(LSNumber* x) {
	double r = atan(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_atan2(LSNumber* x, LSNumber* y) {
	double r = atan2(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return LSNumber::get(r);
}

LSNumber* number_cbrt(LSNumber* x) {
	double r = cbrt(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_ceil(LSNumber* x) {
	double r = ceil(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_cos(LSNumber* x) {
	double r = cos(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_exp(LSNumber* x) {
	double r = exp(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_floor(LSNumber* x) {
	double r = floor(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_hypot(LSNumber* x, LSNumber* y) {
	double r = hypot(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return LSNumber::get(r);
}

LSNumber* number_log(LSNumber* x) {
	double r = log(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_log10(LSNumber* x) {
	double r = log10(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_max(LSNumber* x, LSNumber* y) {
	if (x->value < y->value) {
		if (x->refs == 0) delete x;
		return y;
	} else {
		if (y->refs == 0) delete y;
		return x;
	}
}

LSNumber* number_min(LSNumber* x, LSNumber* y) {
	if (x->value > y->value) {
		if (x->refs == 0) delete x;
		return y;
	} else {
		if (y->refs == 0) delete y;
		return x;
	}
}

LSNumber* number_pow(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return LSNumber::get(r);
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

LSNumber* number_round(LSNumber* x) {
	double r = round(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
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

LSNumber* number_sqrt(LSNumber* x) {
	double r = sqrt(x->value);
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

	static_field("pi", Type::FLOAT, (void*) &Number_pi);
	static_field("e", Type::FLOAT, (void*) &Number_e);
	static_field("phi", Type::FLOAT, (void*) &Number_phi);
	static_field("epsilon", Type::FLOAT, (void*) &Number_epsilon);

	method("abs", {
		{Type::POINTER, Type::FLOAT, {}, (void*) &NumberSTD::abs_ptr},
		{Type::FLOAT, Type::FLOAT, {}, (void*) &NumberSTD::abs_real, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {}, (void*) &NumberSTD::abs_real, Method::NATIVE}
	});

	method("acos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_acos);
	method("asin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_asin);
	method("atan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_atan);
	method("cbrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cbrt);

	method("ceil", {
		{Type::NUMBER, Type::FLOAT, {}, (void*) &NumberSTD::ceil_ptr}
	});

	method("char", {
		{Type::NUMBER, Type::POINTER, {}, (void*) &NumberSTD::char_ptr},
		{Type::FLOAT, Type::STRING, {}, (void*) &NumberSTD::char_real, Method::NATIVE},
		{Type::INTEGER, Type::STRING, {}, (void*) &NumberSTD::char_int, Method::NATIVE}
	});

	method("cos", {
		{Type::NUMBER, Type::FLOAT, {}, (void*) &NumberSTD::cos_ptr}
	});
	method("exp", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_exp);

	method("floor", {
		{Type::NUMBER, Type::INTEGER, {}, (void*) &NumberSTD::floor_ptr}
	});

	method("hypot", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_hypot);
	method("log", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log);
	method("log10", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log10);

	method("max", {
		{Type::POINTER, Type::FLOAT, {Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
	});

	method("min", {
		{Type::POINTER, Type::FLOAT, {Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, (void*) &NumberSTD::min_float_float},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::min_float_float},
	});

	method("pow", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_pow);

	method("round", {
		{Type::NUMBER, Type::INTEGER, {}, (void*) &NumberSTD::round_ptr}
	});

	method("signum", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_signum);

	method("sin", {
		{Type::POINTER, Type::FLOAT, {}, (void*) &NumberSTD::sin_ptr}
	});

	method("sqrt", {
		{Type::NUMBER, Type::FLOAT, {}, (void*) &NumberSTD::sqrt_ptr}
	});
	method("tan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_tan);
	method("toDegrees", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toDegrees);
	method("toRadians", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toRadians);
	method("isInteger", Type::NUMBER, Type::BOOLEAN, {}, (void*) &number_isInteger);

	/*
	 * Static methods
	 */
	static_method("abs", {
		{Type::FLOAT, {Type::POINTER}, (void*) &NumberSTD::abs_ptr},
		{Type::FLOAT, {Type::FLOAT}, (void*) &NumberSTD::abs_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::abs_real, Method::NATIVE}
	});

	static_method("acos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_acos);
	static_method("asin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_asin);
	static_method("atan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_atan);
	static_method("atan2", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_atan2);
	static_method("cbrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cbrt);

	static_method("ceil", {
		{Type::FLOAT_P, {Type::NUMBER}, (void*) &NumberSTD::ceil_ptr},
		{Type::INTEGER, {Type::FLOAT}, (void*) &NumberSTD::ceil_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::ceil_int, Method::NATIVE}
	});

	static_method("char", {
		{Type::STRING, {Type::POINTER}, (void*) &NumberSTD::char_ptr},
		{Type::STRING, {Type::FLOAT}, (void*) &NumberSTD::char_real, Method::NATIVE},
		{Type::STRING, {Type::INTEGER}, (void*) &NumberSTD::char_int, Method::NATIVE}
	});

	static_method("cos", {
		{Type::FLOAT, {Type::NUMBER}, (void*) &NumberSTD::cos_ptr}
	});

	static_method("exp", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_exp);

	static_method("floor", {
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::floor_ptr},
		{Type::INTEGER, {Type::FLOAT}, (void*) &NumberSTD::floor_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::floor_int, Method::NATIVE},
	});

	static_method("hypot", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_hypot);
	static_method("log", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log);
	static_method("log10", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log10);

	static_method("max", {
		{Type::FLOAT, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::FLOAT, {Type::POINTER, Type::FLOAT}, (void*) &NumberSTD::max_ptr_float},
		{Type::FLOAT, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::max_ptr_int},
		{Type::FLOAT, {Type::FLOAT, Type::FLOAT}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::FLOAT, {Type::FLOAT, Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::FLOAT, {Type::INTEGER, Type::FLOAT}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE}
	});

	static_method("min", {
		{Type::FLOAT, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::FLOAT, {Type::POINTER, Type::FLOAT}, (void*) &NumberSTD::min_ptr_float},
		{Type::FLOAT, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::min_ptr_int},
		{Type::FLOAT, {Type::FLOAT, Type::FLOAT}, (void*) &NumberSTD::min_float_float},
		{Type::FLOAT, {Type::FLOAT, Type::INTEGER}, (void*) &NumberSTD::min_float_float},
		{Type::FLOAT, {Type::INTEGER, Type::FLOAT}, (void*) &NumberSTD::min_float_float},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::min_float_float}
	});

	static_method("pow", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_pow);
	static_method("rand", Type::FLOAT, {}, (void*) &number_rand);
	static_method("randFloat", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_randFloat);
	static_method("randInt", Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &number_randInt);

	static_method("round", {
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::round_ptr},
		{Type::INTEGER, {Type::FLOAT}, (void*) &NumberSTD::round_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::round_int, Method::NATIVE}
	});

	static_method("signum", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_signum);

	static_method("sin", {
		{Type::FLOAT, {Type::POINTER}, (void*) &NumberSTD::sin_ptr}
	});

	static_method("sqrt", {
		{Type::FLOAT, {Type::POINTER}, (void*) &NumberSTD::sqrt_ptr}
	});

	static_method("tan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_tan);
	static_method("toDegrees", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toDegrees);
	static_method("toRadians", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toRadians);
	static_method("isInteger", Type::BOOLEAN, {Type::NUMBER}, (void*) &number_isInteger);


}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

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

}
