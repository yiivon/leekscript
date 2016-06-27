#ifndef VM_HPP
#define VM_HPP

#include <string>
#include <jit/jit.h>

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

#define OPERATION_LIMIT 10000000

namespace ls {

class Module;
class Program;

enum class ExecMode {
	NORMAL, TOP_LEVEL, COMMAND_JSON, TEST, TEST_OPS, FILE_JSON
};

class vm_operation_exception : public std::exception {
public:
	virtual const char* what() const throw (){
	   return "too much operations!";
	}
};

class VM {
public:

	static unsigned int operations;
	static const bool enable_operations;
	static const unsigned int operation_limit;

	std::vector<Module*> modules;

	VM();
	virtual ~VM();

	void add_module(Module* m);

	std::string execute(const std::string code, std::string ctx, ExecMode mode);
	Program* compile(const std::string code);

	static jit_value_t value_to_pointer(jit_function_t&, jit_value_t&, Type);
	static jit_value_t pointer_to_value(jit_function_t&, jit_value_t&, Type);
	static jit_value_t new_array(jit_function_t&);
	// static bool is_number(void* v);
	static void push_array_value(jit_function_t&, jit_value_t&, jit_value_t&);
	static void push_array_pointer(jit_function_t&, jit_value_t&, jit_value_t&);

	static jit_value_t get_refs(jit_function_t& F, jit_value_t& obj);
	static void inc_refs(jit_function_t& F, jit_value_t& obj);
	static void delete_obj(jit_function_t& F, jit_value_t& obj);
	static void delete_temporary(jit_function_t& F, jit_value_t& obj);
	static void inc_ops(jit_function_t& F, int add);
	static void get_operations(jit_function_t& F);
	static void print_int(jit_function_t& F, jit_value_t& val);
};

}

#endif
