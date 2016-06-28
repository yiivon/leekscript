#include "../../compiler/value/ArrayAccess.hpp"

#include "../../compiler/value/Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"

using namespace std;

namespace ls {

ArrayAccess::ArrayAccess() {
	array = nullptr;
	key = nullptr;
	key2 = nullptr;
	type = Type::POINTER;
}

ArrayAccess::~ArrayAccess() {
	delete array;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

void ArrayAccess::print(std::ostream& os) const {
	array->print(os);
	os << "[";
	key->print(os);
	if (key2 != nullptr) {
		os << ":";
		key2->print(os);
	}
	os << "]";
}

void ArrayAccess::analyse(SemanticAnalyser* analyser, const Type) {

	array->analyse(analyser);
	key->analyse(analyser);
	constant = array->constant and key->constant;

	if (array->type.raw_type == RawType::ARRAY) {
		type = array->type.getElementType();
	}

//	cout << "array access " << type << endl;
}

bool ArrayAccess::will_take(SemanticAnalyser* analyser, const unsigned pos, const Type arg_type) {

	type.will_take(pos, arg_type);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, pos, arg_type, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, pos, arg_type, 1);
	}

	return false;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyser* analyser, const unsigned pos, const Type arg_type, int level) {

	type.will_take(pos, arg_type);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, pos, arg_type, level);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, pos, arg_type, level + 1);
	}

	return false;
}

LSValue* access_temp(LSArray<LSValue*>* array, LSValue* key) {
	return array->at(key);
}

int access_temp_value(LSArray<int>* array, int key) {
	return array->atv(key);
}

LSValue** access_l(LSArray<LSValue*>* array, LSValue* key) {
	return array->atL(key);
}

int* access_l_value(LSArray<int>* array, int key) {
	return array->atLv(key);
}

LSValue* range(LSArray<LSValue*>* array, int start, int end) {
	return array->range(start, end);
}

jit_value_t ArrayAccess::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

//	cout << "aa " << type << endl;

	jit_value_t a = array->compile_jit(c, F, Type::POINTER);

	if (key2 == nullptr) {

		jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

		Type t = type == Type::INTEGER ? Type::VALUE : Type::POINTER;

		jit_value_t k = key->compile_jit(c, F, t);

		void* func = type == Type::INTEGER ? (void*) access_temp_value : (void*) access_temp;

		jit_value_t args[] = {a, k};
		jit_value_t res = jit_insn_call_native(F, "access", func, sig, args, 2, JIT_CALL_NOTHROW);

		if (t.nature == Nature::POINTER) {
			VM::delete_temporary(F, k);
		}
		VM::delete_temporary(F, a);

		// Array access : 2 operations
		VM::inc_ops(F, 2);

		if (type.nature == Nature::VALUE and req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, res, type);
		}
		return res;

	} else {

		jit_type_t args_types[3] = {JIT_POINTER, JIT_INTEGER, JIT_INTEGER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 3, 0);

		jit_value_t start = key->compile_jit(c, F, Type::INTEGER);
		jit_value_t end = key2->compile_jit(c, F, Type::INTEGER);
		jit_value_t args[] = {a, start, end};

		jit_value_t result = jit_insn_call_native(F, "range", (void*) range, sig, args, 3, JIT_CALL_NOTHROW);

		VM::delete_temporary(F, a);

		return result;
	}
}

jit_value_t ArrayAccess::compile_jit_l(Compiler& c, jit_function_t& F, Type) const {

//	cout << "aal " << type << endl;

	jit_value_t a = array->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

//	jit_value_t k = key->compile_jit(c, F, Type::POINTER);
	jit_value_t k = key->compile_jit(c, F, Type::NEUTRAL);

	jit_value_t args[] = {a, k};
//	return jit_insn_call_native(F, "access_l", (void*) access_l, sig, args, 2, JIT_CALL_NOTHROW);
	return jit_insn_call_native(F, "access_l", (void*) access_l_value, sig, args, 2, JIT_CALL_NOTHROW);
}

}
