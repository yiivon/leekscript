#include "Program.hpp"
#include "../vm/Context.hpp"
#include "../vm/value/LSNull.hpp"

using namespace std;

Program::Program() {
	body = nullptr;
}

Program::~Program() {
	if (body != nullptr) {
		delete body;
	}
}

void Program::print(ostream& os) {
	body->print(os);
	cout << endl;
}

extern map<string, jit_value_t> internals;
extern map<string, jit_value_t> globals;
extern map<string, Type> globals_types;

LSArray* Program_create_array() {
	return new LSArray();
}
void Program_push_null(LSArray* array, int value) {
	array->pushClone(LSNull::null_var);
}
void Program_push_boolean(LSArray* array, int value) {
	array->pushClone(LSBoolean::get(value));
}
void Program_push_integer(LSArray* array, int value) {
	array->pushClone(LSNumber::get(value));
}
void Program_push_float(LSArray* array, float value) {
	array->pushClone(LSNumber::get(value));
}
void Program_push_function(LSArray* array, void* value) {
	array->pushClone(new LSFunction(value));
}
void Program_push_pointer(LSArray* array, LSValue* value) {
	array->pushClone(value);
}

void Program::compile_jit(Compiler& c, jit_function_t& F, Context& context, bool toplevel) {

//	cout << endl << "COMPILE" << endl << endl;

	// System global variables
	for (auto var : system_vars) {

		string name = var.first;
		LSValue* value = var.second;

		//cout << "Add " << name << endl;

//		jit_value_t jit_var = jit_value_create(F, JIT_INTEGER_LONG);

		jit_value_t jit_val = JIT_CREATE_CONST_POINTER(F, value);

		//cout << value << endl;

		internals.insert(pair<string, jit_value_t>(name, jit_val));

//		globals_infos.insert(pair<string, Info>(name, Info::pointer));
//		jit_insn_store(F, jit_var, jit_val);

		//cout << "var : " << jit_val << endl;
	}

	// User context variables
	if (toplevel) {
		for (auto var : context.vars) {

//			cout << "context var " << var.first << endl;

			string name = var.first;
			LSValue* value = var.second;

			jit_value_t jit_var = jit_value_create(F, JIT_POINTER);
			jit_value_t jit_val = JIT_CREATE_CONST_POINTER(F, value);
			jit_insn_store(F, jit_var, jit_val);

//			cout << jit_var << endl;

			globals.insert(pair<string, jit_value_t>(name, jit_var));
			globals_types.insert(pair<string, Type>(name, Type(value->getRawType(), Nature::POINTER)));
		}
	}

//	cout << "execute" << endl;

	jit_value_t res = body->compile_jit(c, F, Type::POINTER);

//	cout << "body type : " << body->type << endl;

//	cout << "res : " << res << endl;

	if (toplevel) {

		// Push program res
		jit_type_t array_sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
		jit_value_t array = jit_insn_call_native(F, "new", (void*) &Program_create_array, array_sig, {}, 0, JIT_CALL_NOTHROW);

		jit_type_t push_args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t push_sig_pointer = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);

		jit_value_t push_args[2] = {array, res};
		jit_insn_call_native(F, "push", (void*) &Program_push_pointer, push_sig_pointer, push_args, 2, 0);


//		cout << "GLOBALS : " << globals.size() << endl;

		for (auto g : globals) {

			string name = g.first;
			Type type = globals_types[name];

//			cout << "save in context : " << name << ", type: " << type << endl;
//			cout << "jit_val: " << g.second << endl;

			jit_value_t var_args[2] = {array, g.second};

			if (type.nature == Nature::POINTER) {

//				cout << "save pointer" << endl;
				jit_insn_call_native(F, "push", (void*) &Program_push_pointer, push_sig_pointer, var_args, 2, 0);

			} else {
//				cout << "save value" << endl;
				if (type.raw_type == RawType::NULLL) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_null, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::BOOLEAN) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_boolean, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::INTEGER) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_integer, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::FLOAT) {
					jit_type_t args_float[2] = {JIT_POINTER, JIT_FLOAT};
					jit_type_t sig_push_float = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args_float, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_float, sig_push_float, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::FUNCTION) {
					jit_insn_call_native(F, "push", (void*) &Program_push_function, push_sig_pointer, var_args, 2, JIT_CALL_NOTHROW);
				}
			}
		}
		jit_insn_return(F, array);
	} else {
		jit_insn_return(F, res);
	}
}
