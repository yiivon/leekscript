#ifndef NUMBER_STD_HPP
#define NUMBER_STD_HPP

#include "../VM.hpp"
#include "../../parser/semantic/SemanticAnalyser.hpp"
#include "../../parser/Program.hpp"
#include "../Module.hpp"
#include <math.h>

class NumberSTD : public Module {
public:
	NumberSTD();
};

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
LSNumber* number_rand();
LSNumber* number_randFloat(const LSNumber* min, const LSNumber* max);
LSNumber* number_randInt(const LSNumber* min, const LSNumber* max);
LSNumber* number_round(const LSNumber* number);
LSNumber* number_signum(const LSNumber* number);
LSNumber* number_sin(const LSNumber* number);
LSNumber* number_sqrt(const LSNumber* number);
LSNumber* number_tan(const LSNumber* number);
LSNumber* number_toDegrees(const LSNumber* number);
LSNumber* number_toRadians(const LSNumber* number);


#endif
