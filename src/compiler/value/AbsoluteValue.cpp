#include "../../compiler/value/AbsoluteValue.hpp"

#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
	type = Type::INTEGER;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os) const {
	os << "|";
	expression->print(os);
	os << "|";
}

int AbsoluteValue::line() const {
	return 0;
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type) {
	expression->analyse(analyser);
	constant = expression->constant;
}

LSValue* abso(LSValue* v) {
	return v->abso();
}

jit_value_t AbsoluteValue::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_value_t ex = expression->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);

	jit_value_t absolute_value = jit_insn_call_native(F, "abso", (void*) abso, sig, &ex, 1, JIT_CALL_NOTHROW);

	VM::delete_temporary(F, ex);

	return absolute_value;
}

}
