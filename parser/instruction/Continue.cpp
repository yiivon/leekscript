#include "Continue.hpp"
#include "../../vm/value/LSNull.hpp"

Continue::Continue() {
	value = 1;
}

Continue::~Continue() {}

void Continue::print(std::ostream& os) const {
	os << "continue";
	if (value > 1) {
		os << " " << value;
	}
}

void Continue::analyse(SemanticAnalyser*, const Type&) {

}

jit_value_t Continue::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_insn_branch(F, c.get_current_loop_cond_label());

	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}
