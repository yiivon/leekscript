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

	static jit_value_t char_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t char_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t char_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t floor_ptr(Compiler&, std::vector<jit_value_t>);
	static jit_value_t floor_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t floor_int(Compiler&, std::vector<jit_value_t>);
};

}

#endif
