#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <jit/jit.h>
#include <vector>
#include <map>
#include <stack>
#include <functional>

#include "../vm/Type.hpp"

namespace ls {

class Program;
class VM;

class CompilerVar {
public:
	jit_value_t value;
	Type type;
	bool reference;
	CompilerVar() : value(jit_value_t{}), type(Type::UNKNOWN), reference(false) {}
	CompilerVar(jit_value_t value, const Type& type, bool reference) :
		value(value), type(type), reference(reference) {}
};

class Compiler {
public:

	struct value {
		jit_value_t v;
		Type t;
		bool operator == (const value& o) const {
			return v == o.v and t == o.t;
		}
		bool operator != (const value& o) const {
			return v != o.v or t != o.t;
		}
	};

	jit_function_t F = nullptr;
	std::stack<jit_function_t> functions;
	std::vector<int> functions_blocks; // how many blocks are open in the current loop

	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<jit_label_t*> loops_end_labels;
	std::vector<jit_label_t*> loops_cond_labels;
	std::vector<std::vector<CompilerVar>> function_variables;
	std::vector<std::map<std::string, CompilerVar>> variables;

	Program* program;
	VM* vm;

	Compiler(VM* vm);
	virtual ~Compiler();

	// Value creation
	value clone(value) const;
	value new_null() const;
	value new_bool(bool b) const;
	value new_integer(int i) const;
	value new_pointer(const void* p) const;
	value new_object() const;
	value new_object_class(value clazz) const;
	value new_mpz() const;

	// Conversions
	value to_int(value) const;

	// Operators wrapping
	void insn_store(value, value) const;
	void insn_store_relative(value, int, value) const;
	value insn_not(value) const;
	value insn_and(value, value) const;
	value insn_or(value, value) const;
	value insn_add(value, value) const;
	value insn_sub(value, value) const;
	value insn_eq(value, value) const;
	value insn_ne(value, value) const;
	value insn_lt(value, value) const;
	value insn_le(value, value) const;
	value insn_gt(value, value) const;
	value insn_ge(value, value) const;
	value insn_mul(value, value) const;
	value insn_div(value, value) const;
	value insn_int_div(value, value) const;
	value insn_bit_and(value, value) const;
	value insn_bit_or(value, value) const;
	value insn_bit_xor(value, value) const;
	value insn_mod(value, value) const;
	value insn_pow(value, value) const;
	value insn_log10(value) const;

	// Value management
	value insn_to_pointer(value v) const;
	value insn_to_bool(value v) const;
	value insn_address_of(value v) const;
	value insn_load(value v, int pos = 0, Type t = Type::POINTER) const;
	value insn_typeof(value v) const;
	value insn_class_of(value v) const;
	void  insn_delete(value v) const;
	void  insn_delete_temporary(value v) const;
	value insn_array_size(value v) const;
	value insn_get_capture(int index, Type type) const;
	void  insn_push_move_array(value array, value element) const;

	// Iterators
	value iterator_begin(value v) const;
	value iterator_end(value v, value it) const;
	value iterator_get(value it) const;
	value iterator_key(value v, value it) const;
	void iterator_increment(value it) const;

	// Controls
	void insn_if(value v, std::function<void()> then) const;
	void insn_throw(value v) const;

	// Call functions
	template <typename R, typename... A>
	value insn_call(Type return_type, std::vector<value> args, R(*func)(A...)) const {
		return insn_call(return_type, args, (void*) func);
	}
	value insn_call(Type return_type, std::vector<value> args, void* func) const;

	// Blocks
	void enter_block();
	void leave_block(jit_function_t F);
	void delete_variables_block(jit_function_t F, int deepness); // delete all variables in the #deepness current blocks
	void enter_function(jit_function_t F);
	void leave_function();
	int get_current_function_blocks() const;
	void delete_function_variables();

	// Variables
	void add_var(const std::string& name, jit_value_t value, const Type& type, bool ref);
	void add_function_var(jit_value_t value, const Type& type);
	CompilerVar& get_var(const std::string& name);
	void set_var_type(std::string& name, const Type& type);
	std::map<std::string, CompilerVar> get_vars();
	void update_var(std::string& name, jit_value_t value, const Type& type);

	// Loops
	void enter_loop(jit_label_t*, jit_label_t*);
	void leave_loop();
	jit_label_t* get_current_loop_end_label(int deepness) const;
	jit_label_t* get_current_loop_cond_label(int deepness) const;
	int get_current_loop_blocks(int deepness) const;
};

}

#endif
