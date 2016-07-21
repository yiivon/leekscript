#include "../../compiler/value/Nulll.hpp"

#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll() {
	type = Type::NULLL;
	constant = true;
}

Nulll::~Nulll() {}

void Nulll::print(ostream& os) const {
	os << "null";
}

void Nulll::analyse(SemanticAnalyser*, const Type) {}

LSValue* Null_create() {
	return new LSNull();
}

jit_value_t Nulll::compile_jit(Compiler&, jit_function_t& F, Type req_type) const {

	if (req_type.nature == Nature::POINTER) {

		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
		return jit_insn_call_native(F, "convert", (void*) Null_create, sig, {}, 0, JIT_CALL_NOTHROW);
	} else {
		return JIT_CREATE_CONST(F, JIT_INTEGER, 0);
	}
}

}
