#include "Continue.hpp"
#include "../../vm/value/LSNull.hpp"

Continue::Continue() {
	value = 1;
}

Continue::~Continue() {}

void Continue::print(ostream& os) const {
	os << "continue";
	if (value > 1) {
		os << " " << value;
	}
}

void Continue::analyse(SemanticAnalyser* analyser, const Type& req_type) {

}

jit_value_t Continue::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_insn_branch(F, c.get_current_loop_cond_label());

	return jit_value_create_nint_constant(F, jit_type_int, (long int) LSNull::null_var);
}
