#include "NumberSTD.hpp"

using namespace std;

NumberSTD::NumberSTD() : Module("Number") {

	attr("pi", Type::FLOAT_P, "3.14159265359");
	attr("e", Type::FLOAT_P, "2.71828182846");
	attr("phi", Type::FLOAT_P, "1.61803398874");

	method("abs", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_abs);
	method("acos", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_acos);
	method("asin", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_asin);
	method("atan", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_atan);
	method("atan2", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_atan2);
	method("cbrt", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_cbrt);
	method("ceil", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_ceil);
	method("cos", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_cos);
	method("exp", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_exp);
	method("floor", Type::INTEGER_P, {Type::FLOAT_P}, (void*) &number_floor);
	method("hypot", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_hypot);
	method("log", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_log);
	method("log10", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_log10);
	method("max", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_max);
	method("min", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_min);
	method("pow", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_pow);
	method("rand", Type::FLOAT_P, {}, (void*) &number_rand);
	method("randFloat", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_randFloat);
	method("randInt", Type::FLOAT_P, {Type::FLOAT_P, Type::FLOAT_P}, (void*) &number_randInt);
	method("round", Type::INTEGER_P, {Type::FLOAT_P}, (void*) &number_round);
	method("signum", Type::INTEGER_P, {Type::FLOAT_P}, (void*) &number_signum);
	method("sin", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_sin);
	method("sqrt", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_sqrt);
	method("tan", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_tan);
	method("toDegrees", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_toDegrees);
	method("toRadians", Type::FLOAT_P, {Type::FLOAT_P}, (void*) &number_toRadians);
}

LSNumber* number_abs(const LSNumber* number) {
	return LSNumber::get(fabs(number->value));
}

LSNumber* number_acos(const LSNumber* number) {
	return LSNumber::get(acos(number->value));
}

LSNumber* number_asin(const LSNumber* number) {
	return LSNumber::get(asin(number->value));
}

LSNumber* number_atan(const LSNumber* number) {
	return LSNumber::get(atan(number->value));
}

LSNumber* number_atan2(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(atan2(x->value, y->value));
}

LSNumber* number_cbrt(const LSNumber* number) {
	return LSNumber::get(cbrt(number->value));
}

LSNumber* number_ceil(const LSNumber* number) {
	return LSNumber::get(ceil(number->value));
}

LSNumber* number_cos(const LSNumber* x) {
	return LSNumber::get(cos(x->value));
}

LSNumber* number_exp(const LSNumber* x) {
	return LSNumber::get(exp(x->value));
}

LSNumber* number_floor(const LSNumber* number) {
	return LSNumber::get(floor(number->value));
}

LSNumber* number_hypot(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(hypot(x->value, y->value));
}

LSNumber* number_log(const LSNumber* x) {
	return LSNumber::get(log(x->value));
}

LSNumber* number_log10(const LSNumber* x) {
	return LSNumber::get(log10(x->value));
}

LSNumber* number_max(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(max(x->value, y->value));
}

LSNumber* number_min(const LSNumber* x, const LSNumber* y) {
	return new LSNumber(min(x->value, y->value));
}

LSNumber* number_pow(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(pow(x->value, y->value));
}

LSNumber* number_rand() {
	return LSNumber::get((double) rand() / RAND_MAX);
}

LSNumber* number_randFloat(const LSNumber* min, const LSNumber* max) {
	return LSNumber::get(min->value + ((double) rand() / RAND_MAX) * max->value);
}

LSNumber* number_randInt(const LSNumber* min, const LSNumber* max) {
	return LSNumber::get(floor(min->value + ((double) rand() / RAND_MAX) * max->value));
}

LSNumber* number_round(const LSNumber* number) {
	return LSNumber::get(round(number->value));
}

LSNumber* number_signum(const LSNumber* x) {
	if (x->value > 0) return LSNumber::get(1);
	if (x->value < 0) return LSNumber::get(-1);
	return LSNumber::get(0);
}

LSNumber* number_sin(const LSNumber* x) {
	return new LSNumber(sin(x->value));
}

LSNumber* number_sqrt(const LSNumber* x) {
	return new LSNumber(sqrt(x->value));
}

LSNumber* number_tan(const LSNumber* x) {
	return new LSNumber(tan(x->value));
}

LSNumber* number_toDegrees(const LSNumber* x) {
	return LSNumber::get((x->value * 180) / M_PI);
}

LSNumber* number_toRadians(const LSNumber* x) {
	return LSNumber::get((x->value * M_PI) / 180);
}
