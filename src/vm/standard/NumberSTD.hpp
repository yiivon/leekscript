#ifndef NUMBER_STD_HPP
#define NUMBER_STD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"
#include "../../vm/value/LSString.hpp"
#include <math.h>

namespace ls {

class LSNumber;

class NumberSTD : public Module {
public:
	NumberSTD();
};

jit_value_t Number_e(jit_function_t& F);
jit_value_t Number_phi(jit_function_t& F);
jit_value_t Number_pi(jit_function_t& F);
jit_value_t Number_epsilon(jit_function_t& F);

LSNumber* number_abs(const LSNumber* number);
LSNumber* number_acos(const LSNumber* number);
LSNumber* number_asin(const LSNumber* number);
LSNumber* number_atan(const LSNumber* number);
LSNumber* number_atan2(const LSNumber* x, const LSNumber* y);
LSNumber* number_cbrt(const LSNumber* number);
LSNumber* number_ceil(const LSNumber* number);
LSNumber* number_cos(const LSNumber* number);
LSNumber* number_exp(const LSNumber* number);
LSNumber* number_floor(const LSNumber* number);
LSNumber* number_hypot(const LSNumber* x, const LSNumber* y);
LSNumber* number_log(const LSNumber* number);
LSNumber* number_log10(const LSNumber* number);
LSNumber* number_max(const LSNumber* x, const LSNumber* y);
LSNumber* number_min(const LSNumber* x, const LSNumber* y);
LSNumber* number_pow(const LSNumber* x, const LSNumber* y);
double number_rand();
LSNumber* number_randFloat(const LSNumber* min, const LSNumber* max);
int number_randInt(const int min, const int max);
LSNumber* number_round(const LSNumber* number);
LSNumber* number_signum(const LSNumber* number);
LSNumber* number_sin(const LSNumber* number);
LSNumber* number_sqrt(const LSNumber* number);
LSNumber* number_tan(const LSNumber* number);
LSNumber* number_toDegrees(const LSNumber* number);
LSNumber* number_toRadians(const LSNumber* number);
bool number_isInteger(const LSNumber* number);
LSString* number_char(const LSNumber* number);
}

#endif
