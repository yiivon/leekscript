#include "BooleanSTD.hpp"


namespace ls {

int boolean_compare_fun(bool a, bool b) {
	if (a) {
		if (not b) return 1;
	} else {
		if (b) return -1;
	}
	return 0;
}

jit_value_t boolean_compare(Compiler& c, std::vector<jit_value_t> args) {
	std::vector<jit_type_t> types = {LS_INTEGER, LS_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, types.data(), 2, 0);
	return jit_insn_call_native(c.F, "compare", (void*) boolean_compare_fun, sig, args.data(), 2, JIT_CALL_NOTHROW);
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
BooleanSTD::BooleanSTD() : Module("Boolean") {

	static_method("compare", Type::INTEGER, {Type::BOOLEAN, Type::BOOLEAN}, (void*) &boolean_compare);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif



}
