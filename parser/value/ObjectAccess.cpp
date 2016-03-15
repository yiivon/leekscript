#include "ObjectAccess.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"

using namespace std;

ObjectAccess::ObjectAccess() {
	object = nullptr;
	type = Type::POINTER;
}

ObjectAccess::~ObjectAccess() {}

void ObjectAccess::print(ostream& os) const {
	object->print(os);
	os << "." << field;
}

void ObjectAccess::analyse(SemanticAnalyser* analyser, const Type) {

	//cout << "Analyse oo : " << field << endl;

	object->analyse(analyser);

	try {
		type = object->attr_types.at(field);
		//cout << "Type of " << field << " : " << type << endl;
	} catch (exception&) {}

}

LSValue* object_access(LSValue* o, LSString* k) {
	return o->attr(k);
}

LSValue** object_access_l(LSValue* o, LSString* k) {
	return o->attrL(k);
}

jit_value_t ObjectAccess::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	jit_value_t o = object->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

	jit_value_t k = JIT_CREATE_CONST_POINTER(F,  new LSString(field));
	jit_value_t args[] = {o, k};
	return jit_insn_call_native(F, "access", (void*) object_access, sig, args, 2, JIT_CALL_NOTHROW);
}

jit_value_t ObjectAccess::compile_jit_l(Compiler& c, jit_function_t& F, Type type) const {

	jit_value_t o = object->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

	jit_value_t k = JIT_CREATE_CONST_POINTER(F,  new LSString(field));
	jit_value_t args[] = {o, k};
	return jit_insn_call_native(F, "access_l", (void*) object_access_l, sig, args, 2, JIT_CALL_NOTHROW);
}

