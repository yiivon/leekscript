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
class Compiler;

class NumberSTD : public Module {
public:
	NumberSTD();

	/*
	 * Operators
	 */
	static jit_value_t add_real_real(Compiler& c, std::vector<jit_value_t>);

	/*
	 * Methods
	 */
	static double abs_ptr(LSNumber* x);
	static jit_value_t abs_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t abs_int(Compiler&, std::vector<jit_value_t>);

	static double acos_ptr(LSNumber* x);
	static jit_value_t acos_real(Compiler&, std::vector<jit_value_t>);

	static LSString* char_ptr(LSNumber* x);
	static jit_value_t char_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t char_int(Compiler&, std::vector<jit_value_t>);

	static int floor_ptr(LSNumber* x);
	static jit_value_t floor_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t floor_int(Compiler&, std::vector<jit_value_t>);

	static int round_ptr(LSNumber* x);
	static jit_value_t round_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t round_int(Compiler&, std::vector<jit_value_t>);

	static int ceil_ptr(LSNumber* x);
	static jit_value_t ceil_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t ceil_int(Compiler&, std::vector<jit_value_t>);

	static double max_ptr_ptr(LSNumber* x, LSNumber* y);
	static double max_ptr_float(LSNumber* x, double y);
	static double max_ptr_int(LSNumber* x, int y);
	static double max_float_ptr(double x, LSNumber* y);
	static jit_value_t max_float_float(Compiler&, std::vector<jit_value_t>);
	static double max_int_ptr(int x, LSNumber* y);

	static double min_ptr_ptr(LSNumber* x, LSNumber* y);
	static double min_ptr_float(LSNumber* x, double y);
	static double min_ptr_int(LSNumber* x, int y);
	static double min_float_ptr(double x, LSNumber* y);
	static jit_value_t min_float_float(Compiler&, std::vector<jit_value_t>);
	static double min_int_ptr(int x, LSNumber* y);

	static double cos_ptr(LSNumber* x);

	static double sin_ptr(LSNumber* x);

	static double sqrt_ptr(LSNumber* x);

	static jit_value_t pow_int(Compiler&, std::vector<jit_value_t>);
};

}

#endif
