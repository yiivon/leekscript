#include "ArrayAccess.hpp"
#include "Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"

using namespace std;

ArrayAccess::ArrayAccess() {
	array = nullptr;
	key = nullptr;
	key2 = nullptr;
	type = Type::POINTER;
}

ArrayAccess::~ArrayAccess() {}

void ArrayAccess::print(ostream& os) const {
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

	if (array->type.raw_type == RawType::ARRAY and array->type.homogeneous) {
		type = array->type.getElementType();
		type.nature = Nature::POINTER;
	}
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

LSValue* access_temp(LSArray* array, LSValue* key) {
	return array->at(key);
}

LSValue** access_l(LSArray* array, LSValue* key) {
	return array->atL(key);
}

LSValue* range(LSArray* array, LSValue* start, LSValue* end) {
	return array->range(start, end);
}

//LSValue* access_int(LSArray* array, int key) {
//	return array->at(key);
//}

jit_value_t ArrayAccess::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_value_t a = array->compile_jit(c, F, Type::POINTER);

	if (key2 == nullptr) {

		jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

		jit_value_t k = key->compile_jit(c, F, Type::POINTER);
		jit_value_t args[] = {a, k};
		return jit_insn_call_native(F, "access", (void*) access_temp, sig, args, 2, JIT_CALL_NOTHROW);

	} else {

		jit_type_t args_types[3] = {JIT_POINTER, JIT_POINTER, JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 3, 0);

		jit_value_t start = key->compile_jit(c, F, Type::POINTER);
		jit_value_t end = key2->compile_jit(c, F, Type::POINTER);
		jit_value_t args[] = {a, start, end};
		return jit_insn_call_native(F, "range", (void*) range, sig, args, 3, JIT_CALL_NOTHROW);
	}
}

jit_value_t ArrayAccess::compile_jit_l(Compiler& c, jit_function_t& F, Type) const {

	jit_value_t a = array->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

	jit_value_t k = key->compile_jit(c, F, Type::POINTER);
	jit_value_t args[] = {a, k};
	return jit_insn_call_native(F, "access_l", (void*) access_l, sig, args, 2, JIT_CALL_NOTHROW);
}
