/*
 * for (let k : let v in array) { ... }
 */
#include "Foreach.hpp"
#include "../value/Array.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

Foreach::Foreach() {
	key = nullptr;
	value = nullptr;
	body = nullptr;
	array = nullptr;
	key_var = nullptr;
	value_var = nullptr;
}

Foreach::~Foreach() {}

void Foreach::print(ostream& os) const {
	os << "for ";

	os << "let ";
	if (key != nullptr) {
		os << key;
		os << " : let ";
	}
	os << value;

	os << " in ";
	array->print(os);

	os << " do" << endl;
	body->print(os);
	os << "end";
}

void Foreach::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	array->analyse(analyser, Type::NEUTRAL);

	var_type = Type::POINTER;
	/*
	if (Array* a = dynamic_cast<Array*>(array)) {
		if (a->only_values) {
			var_type = Type::VALUE;
		}
	}
	*/

	if (key != nullptr) {
		key_var = analyser->add_var(key, Type::POINTER, nullptr);
	}

	value_var = analyser->add_var(value, var_type, nullptr);

	body->analyse(analyser, req_type);
}

extern map<string, jit_value_t> globals;

LSArrayIterator get_array_begin(LSArray* a) {
	return a->begin();
}

LSArrayIterator get_array_end(LSArray* a) {
	return a->end();
}

LSValue* get_array_elem(LSArrayIterator it) {
	return it->second;
}

LSValue* get_array_key(LSArrayIterator it) {
	return it->first;
}

int get_array_elem_int(LSArray* a, int i) {
	LSValue* v = a->at(LSNumber::get(i));
	return (int) ((LSNumber*) v)->value;
}

LSArrayIterator iterator_inc(LSArrayIterator it) {
	return ++it;
}

jit_value_t Foreach::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	// Labels
	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	c.enter_loop(&label_end, &label_cond);

	// Array
	jit_value_t a = array->compile_jit(c, F, Type::NEUTRAL);

	// Variable it = begin()
	jit_value_t it = jit_value_create(F, JIT_POINTER);
	jit_type_t args_types_begin[1] = {JIT_POINTER};
	jit_type_t sig_begin = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_begin, 1, 0);
	jit_insn_store(F, it, jit_insn_call_native(F, "begin", (void*) get_array_begin, sig_begin, &a, 1, JIT_CALL_NOTHROW));

	// cond label:
	jit_insn_label(F, &label_cond);

	// Get array end()
	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
	jit_value_t end = jit_insn_call_native(F, "end", (void*) get_array_end, sig, &a, 1, JIT_CALL_NOTHROW);

	// if (i == size) jump to end
	jit_value_t cmp = jit_insn_eq(F, it, end);
	jit_insn_branch_if(F, cmp, &label_end);

	// Get array element (each value of array)
	jit_value_t value_val;
	if (var_type.nature == Nature::POINTER) {
//		cout << "compile pointer" << endl;
		jit_type_t args_types[1] = {JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
		value_val = jit_insn_call_native(F, "get", (void*) get_array_elem, sig, &it, 1, JIT_CALL_NOTHROW);
	} else {
		jit_type_t args_types[2] = {JIT_POINTER, JIT_INTEGER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 2, 0);
		value_val = jit_insn_call_native(F, "get", (void*) get_array_elem_int, sig, &it, 2, JIT_CALL_NOTHROW);
	}

	jit_value_t value_var = jit_value_create(F, JIT_POINTER);
	jit_insn_store(F, value_var, value_val);
	globals.insert(pair<string, jit_value_t>(value->content, value_var));

	// Key
	if (key != nullptr) {

		jit_type_t args_types[1] = {JIT_POINTER};
		jit_type_t sig2 = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
		jit_value_t key_val = jit_insn_call_native(F, "get", (void*) get_array_key, sig2, &it, 1, JIT_CALL_NOTHROW);

		jit_value_t key_var = jit_value_create(F, JIT_POINTER);
		jit_insn_store(F, key_var, key_val);
		globals.insert(pair<string, jit_value_t>(key->content, key_var));
	}

	// body
	body->compile_jit(c, F, Type::NEUTRAL);

	// it++
	jit_type_t args_types_3[1] = {JIT_POINTER};
	jit_type_t sig3 = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_3, 1, 0);
	jit_insn_store(F, it, jit_insn_call_native(F, "inc", (void*) iterator_inc, sig3, &it, 1, JIT_CALL_NOTHROW));

	// jump to cond
	jit_insn_branch(F, &label_cond);

	// end label:
	jit_insn_label(F, &label_end);

	c.leave_loop();

	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}
