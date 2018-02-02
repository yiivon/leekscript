#ifndef LIBJIT_COMPILER_HPP
#define LIBJIT_COMPILER_HPP

#include <jit/jit.h>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include "../vm/Type.hpp"
#include "../vm/Exception.hpp"
#include "../vm/LSValue.hpp"
#include <gmp.h>

namespace ls {

class Program;
class VM;
class Function;

class LibJITCompiler {
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
		value() : v(nullptr), t(Type::UNKNOWN) {}
		value(jit_value_t v, Type t) : v(v), t(t) {}
	};
	struct label {
		jit_label_t l = jit_label_undefined;
	};
	struct catcher {
		label start;
		label end;
		label handler;
		label next;
	};

	jit_function_t F = nullptr;
	std::stack<jit_function_t> functions;
	std::stack<bool> function_is_closure;
	std::vector<int> functions_blocks; // how many blocks are open in the current loop
	std::stack<std::vector<std::string>> arg_names;

	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<label*> loops_end_labels;
	std::vector<label*> loops_cond_labels;
	std::vector<std::vector<value>> function_variables;
	std::vector<std::map<std::string, value>> variables;
	std::vector<std::vector<catcher>> catchers;
	bool output_assembly = false;
	std::ostringstream assembly;
	bool output_pseudo_code = false;
	std::ostringstream pseudo_code;

	bool log_instructions = false;
	std::ostringstream instructions_debug;
	std::map<label*, std::string> label_map;
	std::map<jit_value_t, std::string> var_map;
	std::map<void*, std::string> literals;

	Program* program;
	VM* vm;

	LibJITCompiler(VM* vm);
	virtual ~LibJITCompiler();

	// Value creation
	value clone(value) const;
	value new_null() const;
	value new_bool(bool b) const;
	value new_integer(int i) const;
	value new_real(double r) const;
	value new_long(long l) const;
	value new_pointer(const void* p) const;
	value new_object() const;
	value new_object_class(value clazz) const;
	value new_mpz(long value = 0) const;
	value new_mpz_init(const mpz_t mpz_value) const;

	// Conversions
	value to_int(value) const;
	value to_real(value) const;
	value to_long(value) const;
	value insn_convert(value, Type) const;

	// Operators wrapping
	value insn_not(value) const;
	value insn_not_bool(value) const;
	value insn_neg(value) const;
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
	value insn_create_value(Type t) const;
	value insn_to_pointer(value v) const;
	value insn_to_bool(value v) const;
	value insn_address_of(value v) const;
	value insn_load(value v, int pos = 0, Type t = Type::POINTER) const;
	void  insn_store(value, value) const;
	void  insn_store_relative(value, int, value) const;
	value insn_typeof(value v) const;
	value insn_class_of(value v) const;
	void  insn_delete(value v) const;
	void  insn_delete_temporary(value v) const;
	value insn_get_capture(int index, Type type) const;
	value insn_move_inc(value) const;
	value insn_clone_mpz(value mpz) const;
	void  insn_delete_mpz(value mpz) const;
	value insn_inc_refs(value v) const;
	value insn_dec_refs(value v, value previous = {nullptr, Type::NULLL}) const;
	value insn_move(value v) const;
	value insn_refs(value v) const;
	value insn_native(value v) const;

	// Arrays
	value new_array(Type element_type, std::vector<value> elements) const;
	value insn_array_size(value v) const;
	void  insn_push_array(value array, value element) const;
	value insn_array_at(value array, value index) const;

	// Iterators
	value iterator_begin(value v) const;
	value iterator_end(value v, value it) const;
	value iterator_get(value it, value previous) const;
	value iterator_key(value v, value it, value previous) const;
	void iterator_increment(value it) const;

	// Controls
	void insn_if(value v, std::function<void()> then) const;
	void insn_if_not(value v, std::function<void()> then) const;
	void insn_throw(value v) const;
	void insn_throw_object(vm::Exception type) const;
	void insn_label(label*) const;
	void insn_branch(label* l) const;
	void insn_branch_if(value v, label* l) const;
	void insn_branch_if_not(value v, label* l) const;
	void insn_branch_if_pc_not_in_range(label* a, label* b, label* n) const;
	void insn_return(value v) const;

	// Call functions
	template <typename R, typename... A>
	value insn_call(Type return_type, std::vector<value> args, R(*func)(A...), std::string name = "") const {
		return insn_call(return_type, args, (void*) func, std::move(name));
	}
	value insn_call(Type return_type, std::vector<value> args, void* func, std::string name = "") const;
	value insn_call_indirect(Type return_type, value fun, std::vector<value> args) const;
	void function_add_capture(value fun, value capture);
	void log(const std::string&& str) const;

	// Blocks
	void enter_block();
	void leave_block();
	void delete_variables_block(int deepness); // delete all variables in the #deepness current blocks
	void enter_function(jit_function_t F, bool is_closure, Function* fun);
	void leave_function();
	int get_current_function_blocks() const;
	void delete_function_variables();
	bool is_current_function_closure() const;

	// Variables
	void add_var(const std::string& name, value value);
	void add_function_var(value value);
	value& get_var(const std::string& name);
	void set_var_type(std::string& name, const Type& type);
	std::map<std::string, value> get_vars();
	void update_var(std::string& name, value value);

	// Loops
	void enter_loop(label*, label*);
	void leave_loop();
	label* get_current_loop_end_label(int deepness) const;
	label* get_current_loop_cond_label(int deepness) const;
	int get_current_loop_blocks(int deepness) const;

	/** Operations **/
	void inc_ops(int add) const;
	void inc_ops_jit(value add) const;
	void get_operations() const;

	/** Exceptions **/
	void mark_offset(int line);
	void add_catcher(label start, label end, label handler);
	void insn_check_args(std::vector<value> args, std::vector<LSValueType> types) const;

	// Utils
	std::ostringstream& _log_insn(int indent) const;
	std::string dump_val(value v) const;
	void register_label(label* v) const;
	void log_insn_code(std::string instruction) const;
	void add_literal(void* ptr, std::string value) const;
};

}

#endif
