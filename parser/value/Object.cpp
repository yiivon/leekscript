#include "Object.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSObject.hpp"

using namespace std;

Object::Object() {
	type = Type::OBJECT;
}

Object::~Object() {}

void Object::print(ostream& os) const {
	os << "{";
	for (unsigned i = 0; i < keys.size(); ++i) {
		os << keys.at(i)->token->content;
		os << ": ";
		values.at(i)->print(os);
		if (i < keys.size() - 1) {
			os << ", ";
		}
	}
	os << "}";
}

void Object::analyse(SemanticAnalyser* analyser, const Type) {
	for (Value* value : values) {
		value->analyse(analyser);
	}
}

void push_object(LSObject* o, LSString* k, LSValue* v) {
	o->addField(k->value, v);
}

jit_value_t Object::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	LSObject* o = new LSObject();
	jit_value_t object = JIT_CREATE_CONST_POINTER(F, o);

	jit_type_t args[3] = {JIT_POINTER, JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 3, 0);

	for (unsigned i = 0; i < keys.size(); ++i) {
		jit_value_t k = JIT_CREATE_CONST_POINTER(F, new LSString(keys.at(i)->token->content));
		jit_value_t v = values[i]->compile_jit(c, F, Type::POINTER);
		jit_value_t args[] = {object, k, v};
		jit_insn_call_native(F, "push", (void*) push_object, sig, args, 3, JIT_CALL_NOTHROW);
	}

	return object;
}
