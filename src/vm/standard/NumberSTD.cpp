#include "NumberSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../../../lib/utf8.h"

using namespace std;

namespace ls {

jit_value_t Number_e(jit_function_t F);
jit_value_t Number_phi(jit_function_t F);
jit_value_t Number_pi(jit_function_t F);
jit_value_t Number_epsilon(jit_function_t F);

LSNumber* number_abs(LSNumber* number);
LSNumber* number_acos(LSNumber* number);
LSNumber* number_asin(LSNumber* number);
LSNumber* number_atan(LSNumber* number);
LSNumber* number_atan2(LSNumber* x, LSNumber* y);
LSNumber* number_cbrt(LSNumber* number);
LSNumber* number_ceil(LSNumber* number);
LSNumber* number_cos(LSNumber* number);
LSNumber* number_exp(LSNumber* number);
LSNumber* number_floor(LSNumber* number);
LSNumber* number_hypot(LSNumber* x, LSNumber* y);
LSNumber* number_log(LSNumber* number);
LSNumber* number_log10(LSNumber* number);
LSNumber* number_max(LSNumber* x, LSNumber* y);
LSNumber* number_min(LSNumber* x, LSNumber* y);
LSNumber* number_pow(LSNumber* x, LSNumber* y);
double number_rand();
LSNumber* number_randFloat(LSNumber* min, LSNumber* max);
int number_randInt(const int min, const int max);
LSNumber* number_round(LSNumber* number);
LSNumber* number_signum(LSNumber* number);
LSNumber* number_sin(LSNumber* number);
LSNumber* number_sqrt(LSNumber* number);
LSNumber* number_tan(LSNumber* number);
LSNumber* number_toDegrees(LSNumber* number);
LSNumber* number_toRadians(LSNumber* number);
bool number_isInteger(LSNumber* number);
LSString* number_char(LSNumber* number);

NumberSTD::NumberSTD() : Module("Number") {

	static_field("pi", Type::FLOAT, (void*) &Number_pi);
	static_field("e", Type::FLOAT, (void*) &Number_e);
	static_field("phi", Type::FLOAT, (void*) &Number_phi);
	static_field("epsilon", Type::FLOAT, (void*) &Number_epsilon);

	method("abs", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_abs);
	method("acos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_acos);
	method("asin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_asin);
	method("atan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_atan);
	method("cbrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cbrt);
	method("ceil", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_ceil);
	method("cos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cos);
	method("exp", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_exp);
	method("floor", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_floor);
	method("hypot", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_hypot);
	method("log", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log);
	method("log10", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log10);
	method("max", {
		{Type::NUMBER, Type::NUMBER, {Type::NUMBER}, (void*) &number_max},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, nullptr},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, nullptr},
	});
	method("min", {
		{Type::NUMBER, Type::NUMBER, {Type::NUMBER}, (void*) &number_min},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, nullptr},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, nullptr},
	});
	method("pow", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_pow);
	method("round", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_round);
	method("signum", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_signum);
	method("sin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_sin);
	method("sqrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_sqrt);
	method("tan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_tan);
	method("toDegrees", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toDegrees);
	method("toRadians", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toRadians);
	method("isInteger", Type::NUMBER, Type::BOOLEAN, {}, (void*) &number_isInteger);
	method("char", Type::NUMBER, Type::STRING, {}, (void*) &number_char);

	static_method("abs", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_abs);
	static_method("acos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_acos);
	static_method("asin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_asin);
	static_method("atan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_atan);
	static_method("atan2", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_atan2);
	static_method("cbrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cbrt);
	static_method("ceil", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_ceil);
	static_method("cos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cos);
	static_method("exp", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_exp);
	static_method("floor", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_floor);
	static_method("hypot", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_hypot);
	static_method("log", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log);
	static_method("log10", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log10);
	static_method("max", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_max);
	static_method("min", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_min);
	static_method("pow", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_pow);
	static_method("rand", Type::FLOAT, {}, (void*) &number_rand);
	static_method("randFloat", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_randFloat);
	static_method("randInt", Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &number_randInt);
	static_method("round", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_round);
	static_method("signum", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_signum);
	static_method("sin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_sin);
	static_method("sqrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_sqrt);
	static_method("tan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_tan);
	static_method("toDegrees", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toDegrees);
	static_method("toRadians", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toRadians);
	static_method("isInteger", Type::BOOLEAN, {Type::NUMBER}, (void*) &number_isInteger);
	static_method("char", Type::STRING, {Type::NUMBER}, (void*) &number_char);
}

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

LSNumber* number_abs(LSNumber* number) {
	LSNumber* r = LSNumber::get(fabs(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_acos(LSNumber* number) {
	LSNumber* r = LSNumber::get(acos(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_asin(LSNumber* number) {
	LSNumber* r = LSNumber::get(asin(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_atan(LSNumber* number) {
	LSNumber* r = LSNumber::get(atan(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_atan2(LSNumber* x, LSNumber* y) {
	LSNumber* r = LSNumber::get(atan2(x->value, y->value));
	if (x->refs == 0) {
		delete x;
	}
	if (y->refs == 0) {
		delete y;
	}
	return r;
}

LSNumber* number_cbrt(LSNumber* number) {
	LSNumber* r = LSNumber::get(cbrt(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_ceil(LSNumber* number) {
	LSNumber* r = LSNumber::get(ceil(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_cos(LSNumber* x) {
	LSNumber* r = LSNumber::get(cos(x->value));
	if (x->refs == 0) {
		delete x;
	}
	return r;
}

LSNumber* number_exp(LSNumber* x) {
	LSNumber* r = LSNumber::get(exp(x->value));
	if (x->refs == 0) {
		delete x;
	}
	return r;
}

LSNumber* number_floor(LSNumber* number) {
	LSNumber* r = LSNumber::get(floor(number->value));
	if (number->refs == 0) {
		delete number;
	}
	return r;
}

LSNumber* number_hypot(LSNumber* x, LSNumber* y) {
	double r = hypot(x->value, y->value);
	if (x->refs == 0) {
		if (y->refs == 0) {
			delete y;
		}
		x->value = r;
		return x;
	} else if (y->refs == 0) {
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
	double r = max(x->value, y->value);
	if (x->refs == 0) {
		if (y->refs == 0) {
			delete y;
		}
		x->value = r;
		return x;
	} else if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return LSNumber::get(r);
}

LSNumber* number_min(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	if (x->refs == 0) {
		if (y->refs == 0) {
			delete y;
		}
		x->value = r;
		return x;
	} else if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return LSNumber::get(r);
}

LSNumber* number_pow(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	if (x->refs == 0) {
		if (y->refs == 0) {
			delete y;
		}
		x->value = r;
		return x;
	} else if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return LSNumber::get(r);
}

double number_rand() {
	return (double) rand() / RAND_MAX;
}

LSNumber* number_randFloat(LSNumber* min, LSNumber* max) {
	LSNumber* r = LSNumber::get(min->value + ((double) rand() / RAND_MAX) * max->value);
	if (min->refs == 0) {
		delete min;
	}
	if (max->refs == 0) {
		delete max;
	}
	return r;
}

int number_randInt(int min, int max) {
	return floor(min + ((double) rand() / RAND_MAX) * (max - min));
}

LSNumber* number_round(LSNumber* number) {
	if (number->refs == 0) {
		number->value = round(number->value);
		return number;
	}
	return LSNumber::get(round(number->value));
}

LSNumber* number_signum(LSNumber* x) {
	if (x->value > 0) return LSNumber::get(1);
	if (x->value < 0) return LSNumber::get(-1);
	return LSNumber::get(0);
}

LSNumber* number_sin(LSNumber* x) {
	if (x->refs == 0) {
		x->value = sin(x->value);
		return x;
	}
	return new LSNumber(sin(x->value));
}

LSNumber* number_sqrt(LSNumber* x) {
	if (x->refs == 0) {
		x->value = sqrt(x->value);
		return x;
	}
	return new LSNumber(sqrt(x->value));
}

LSNumber* number_tan(LSNumber* x) {
	if (x->refs == 0) {
		x->value = tan(x->value);
		return x;
	}
	return new LSNumber(tan(x->value));
}

LSNumber* number_toDegrees(LSNumber* x) {
	if (x->refs == 0) {
		x->value = (x->value * 180) / M_PI;
		return x;
	}
	return LSNumber::get((x->value * 180) / M_PI);
}

LSNumber* number_toRadians(LSNumber* x) {
	if (x->refs == 0) {
		x->value = (x->value * M_PI) / 180;
		return x;
	}
	return LSNumber::get((x->value * M_PI) / 180);
}

bool number_isInteger(LSNumber* x) {
	bool is = x->value == (int) x->value;
	if (x->refs == 0) {
		delete x;
	}
	return is;
}

LSString* number_char(LSNumber* number) {
	unsigned int n = number->value;
	if (number->refs == 0) {
		delete number;
	}
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

}
