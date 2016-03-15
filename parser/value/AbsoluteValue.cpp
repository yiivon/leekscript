#include "AbsoluteValue.hpp"

using namespace std;

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
}

AbsoluteValue::~AbsoluteValue() {}

void AbsoluteValue::print(ostream& os) const {
	os << "|";
	expression->print(os);
	os << "|";
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type) {
	expression->analyse(analyser);
	constant = expression->constant;
}

LSValue* abso(LSValue* v) {
	return v->abso();
}

jit_value_t AbsoluteValue::compile_jit(Compiler& c, jit_function_t& F, Type type) const {

	jit_value_t ex = expression->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);

	return jit_insn_call_native(F, "abso", (void*) abso, sig, &ex, 1, JIT_CALL_NOTHROW);
}
