#ifndef VM_HPP
#define VM_HPP

#include <string>
#include <jit/jit.h>

#include "value/LSNull.hpp"
#include "value/LSBoolean.hpp"
#include "value/LSNumber.hpp"
#include "value/LSString.hpp"
#include "value/LSArray.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "value/LSClass.hpp"
#include "Type.hpp"

#define USE_INTEGERS 1

#define JIT_INTEGER jit_type_int
#define JIT_INTEGER_LONG jit_type_long
#define JIT_FLOAT jit_type_float64
#define JIT_POINTER jit_type_void_ptr

#define JIT_CREATE_CONST jit_value_create_nint_constant
#define JIT_CREATE_CONST_LONG jit_value_create_long_constant
#define JIT_CREATE_CONST_FLOAT jit_value_create_float64_constant
#define JIT_CREATE_CONST_POINTER(F, X) jit_value_create_constant((F), new jit_constant_t{ JIT_POINTER, {(X)} })

enum class ExecMode {
	NORMAL, TOP_LEVEL, COMMAND_JSON
};

class VM {
public:

	static std::map<int, void*> globals_vars;
	static void add_global_var(int, void*);

	VM();
	virtual ~VM();

	std::string execute(const std::string code, std::string ctx, ExecMode mode);

	static jit_value_t value_to_pointer(jit_function_t&, jit_value_t&, Type);
	static jit_value_t new_array(jit_function_t&);
	// static bool is_number(void* v);
	static void push_array_value(jit_function_t&, jit_value_t&, jit_value_t&);
	static void push_array_pointer(jit_function_t&, jit_value_t&, jit_value_t&);
};

#endif
