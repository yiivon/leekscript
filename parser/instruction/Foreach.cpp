#include "Foreach.hpp"
#include "../value/Array.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

Foreach::Foreach() {
	body = nullptr;
	array = nullptr;
}

Foreach::~Foreach() {}

void Foreach::print(ostream& os) const {
	os << "for ";

	os << "let ";
	if (key != "") {
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
	if (Array* a = dynamic_cast<Array*>(array)) {
		if (a->only_values) {
			var_type = Type::VALUE;
		}
	}
	analyser->add_var(value, var_type, nullptr);

	body->analyse(analyser, req_type);
}

extern map<string, jit_value_t> globals;

int get_array_size(LSArray* a) {
//	cout << a->values.size() << endl;
//	a->print(cout);
//	cout << endl;
	return a->values.size();
}

LSValue* get_array_elem(LSArray* a, int i) {
//	cout << "get_array_elem " << i << endl;
	LSValue* v = a->at(LSNumber::get(i));
//	v->print(cout);
//	cout << endl;
	return v;
}

int get_array_elem_int(LSArray* a, int i) {
//	cout << "get_array_elem " << i << endl;
	LSValue* v = a->at(LSNumber::get(i));
//	v->print(cout);
//	cout << endl;
	return (int) ((LSNumber*) v)->value;
}

jit_value_t Foreach::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	// Labels
	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	c.enter_loop(&label_end, &label_cond);

	// Variable i = 0
	jit_value_t i = jit_value_create(F, jit_type_int);
	jit_insn_store(F, i, jit_value_create_nint_constant(F, jit_type_int, 0));

	// Array
	jit_value_t a = array->compile_jit(c, F, Type::NEUTRAL);

	// Get array size
	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 1, 0);
	jit_value_t size = jit_insn_call_native(F, "size", (void*) get_array_size, sig, &a, 1, JIT_CALL_NOTHROW);


	// cond label:
	jit_insn_label(F, &label_cond);

	// if (i == size) jump to end
	jit_value_t cmp = jit_insn_eq(F, i, size);
	jit_insn_branch_if(F, cmp, &label_end);

	// Get array element (each value of array)
	jit_type_t args_types_2[2] = {JIT_POINTER, JIT_INTEGER};
	vector<jit_value_t> args = {a, i};
	jit_value_t var;
	if (var_type.nature == Nature::POINTER) {
		jit_type_t sig2 = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_2, 2, 0);
		var = jit_insn_call_native(F, "get", (void*) get_array_elem, sig2, args.data(), 2, JIT_CALL_NOTHROW);
	} else {
		jit_type_t sig2 = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types_2, 2, 0);
		var = jit_insn_call_native(F, "get", (void*) get_array_elem_int, sig2, args.data(), 2, JIT_CALL_NOTHROW);
	}

	globals.insert(pair<string, jit_value_t>(value, var));

	// body
	body->compile_jit(c, F, Type::NEUTRAL);

	// i++
	jit_insn_store(F, i, jit_insn_add(F, i, JIT_CREATE_CONST(F, JIT_INTEGER, 1)));

	// jump to cond
	jit_insn_branch(F, &label_cond);

	// end label:
	jit_insn_label(F, &label_end);

	c.leave_loop();

	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}
