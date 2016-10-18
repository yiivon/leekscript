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

	static jit_value_t abs_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t abs_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t abs_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t char_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t char_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t char_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t floor_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t floor_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t floor_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t round_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t round_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t round_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t ceil_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t ceil_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t ceil_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t max_ptr_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t max_ptr_float(Compiler&, std::vector<jit_value_t>);
	static jit_value_t max_ptr_int(Compiler&, std::vector<jit_value_t>);
	static jit_value_t max_float_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t max_float_float(Compiler&, std::vector<jit_value_t>);
	static jit_value_t max_int_ptr(Compiler&, std::vector<jit_value_t>);

	static jit_value_t min_ptr_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t min_ptr_float(Compiler&, std::vector<jit_value_t>);
	static jit_value_t min_ptr_int(Compiler&, std::vector<jit_value_t>);
	static jit_value_t min_float_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t min_float_float(Compiler&, std::vector<jit_value_t>);
	static jit_value_t min_int_ptr(Compiler&, std::vector<jit_value_t>);

	static jit_value_t cos_ptr(Compiler&, std::vector<jit_value_t>);

	static jit_value_t sin_ptr(Compiler&, std::vector<jit_value_t>);

	static jit_value_t sqrt_ptr(Compiler&, std::vector<jit_value_t>);
};

}

#endif
