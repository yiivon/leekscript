/*
 * for (let k : let v in array) { ... }
 */
#include "../../compiler/instruction/Foreach.hpp"

#include "../../compiler/value/Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"

using namespace std;

namespace ls {

Foreach::Foreach() {
	key = nullptr;
	value = nullptr;
	body = nullptr;
	array = nullptr;
	key_var = nullptr;
	value_var = nullptr;
}

Foreach::~Foreach() {
	delete array;
	delete body;
}

void Foreach::print(ostream& os, int indent, bool debug) const {
	os << tabs(indent) << "for ";

	os << "let ";
	if (key != nullptr) {
		os << key->content;
		os << " : let ";
	}
	os << value->content;

	os << " in ";
	array->print(os, debug);

	os << " ";
	body->print(os, indent, debug);
}

void Foreach::analyse(SemanticAnalyser* analyser, const Type&) {

	analyser->enter_block();

	array->analyse(analyser);

	var_type = array->type.getElementType();

	if (key != nullptr) {
		key_var = analyser->add_var(key, Type::POINTER, nullptr, nullptr);
	}

	value_var = analyser->add_var(value, var_type, nullptr, nullptr);

	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();
	analyser->leave_block();
}

LSArrayIterator<LSValue*> get_array_begin(LSArray<LSValue*>* a) {
	return a->begin();
}
LSArrayIterator<LSValue*> get_array_end(LSArray<LSValue*>* a) {
	return a->end();
}

LSValue* get_array_elem(LSArrayIterator<LSValue*> it) {
	return *it;
}
LSValue* get_array_key(LSArrayIterator<LSValue*> it) {
	return *it;
}
int get_array_elem_int(LSArrayIterator<int> it) {
	return *it;
}
int get_array_key_int(LSArrayIterator<int> it) {
	return *it;
}
double get_array_elem_real(LSArrayIterator<double> it) {
	return *it;
}

LSArrayIterator<LSValue*> iterator_inc(LSArrayIterator<LSValue*> it) {
	return ++it;
}
LSArrayIterator<int> iterator_inc_int(LSArrayIterator<int> it) {
	return ++it;
}
LSArrayIterator<double> iterator_inc_real(LSArrayIterator<double> it) {
	return ++it;
}

jit_value_t Foreach::compile(Compiler& c) const {

	// Labels
	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	c.enter_loop(&label_end, &label_cond);

	// Array
	jit_value_t a = array->compile(c);

	// Variable it = begin()
	jit_value_t it = jit_value_create(c.F, JIT_POINTER);
	jit_type_t args_types_begin[1] = {JIT_POINTER};
	jit_type_t sig_begin = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_begin, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "begin", (void*) get_array_begin, sig_begin, &a, 1, JIT_CALL_NOTHROW));

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// Get array end()
	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
	jit_value_t end = jit_insn_call_native(c.F, "end", (void*) get_array_end, sig, &a, 1, JIT_CALL_NOTHROW);

	// if (i == size) jump to end
	jit_value_t cmp = jit_insn_eq(c.F, it, end);
	jit_insn_branch_if(c.F, cmp, &label_end);

	// Get array element (each value of array)
	jit_value_t value_val = nullptr;
	jit_type_t args_types_val[1] = {JIT_POINTER};
	if (var_type.nature == Nature::POINTER) {
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_val, 1, 0);
		value_val = jit_insn_call_native(c.F, "get", (void*) get_array_elem, sig, &it, 1, JIT_CALL_NOTHROW);
	} else if (var_type.raw_type == RawType::INTEGER) {
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types_val, 1, 0);
		value_val = jit_insn_call_native(c.F, "get", (void*) get_array_elem_int, sig, &it, 1, JIT_CALL_NOTHROW);
	} else if (var_type.raw_type == RawType::FLOAT) {
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, ls_jit_real, args_types_val, 1, 0);
		value_val = jit_insn_call_native(c.F, "get", (void*) get_array_elem_real, sig, &it, 1, JIT_CALL_NOTHROW);
	}

	jit_value_t value_var = jit_value_create(c.F, VM::get_jit_type(var_type));
	jit_insn_store(c.F, value_var, value_val);
	c.add_var(value->content, value_var, var_type, true);

	// Key
	if (key != nullptr) {

		jit_type_t args_types_key[1] = {JIT_POINTER};
		jit_type_t sig2 = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_key, 1, 0);
		jit_value_t key_val = jit_insn_call_native(c.F, "get", (void*) get_array_key, sig2, &it, 1, JIT_CALL_NOTHROW);

		jit_value_t key_var = jit_value_create(c.F, JIT_POINTER);
		jit_insn_store(c.F, key_var, key_val);
		c.add_var(key->content, key_var, Type::POINTER, false);
	}

	// body
	body->compile(c);

	// it++
	jit_type_t args_types_3[1] = {JIT_POINTER};
	jit_type_t sig3 = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_3, 1, 0);
	void* inc_func = (void*) iterator_inc;
	if (var_type.raw_type == RawType::INTEGER) {
		inc_func = (void*) iterator_inc_int;
	} else if (var_type.raw_type == RawType::FLOAT) {
		inc_func = (void*) iterator_inc_real;
	}
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "inc", inc_func, sig3, &it, 1, JIT_CALL_NOTHROW));

	// jump to cond
	jit_insn_branch(c.F, &label_cond);

	// end label:
	jit_insn_label(c.F, &label_end);

	c.leave_loop();

	VM::delete_temporary(c.F, a);

	if (type != Type::VOID) {
		return VM::create_null(c.F);
	}
	return jit_value_create_nint_constant(c.F, jit_type_int, 0);
}

}
