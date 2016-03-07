#include "Break.hpp"
#include "../../vm/value/LSNull.hpp"

Break::Break() {
	value = 1;
}

Break::~Break() {}

void Break::print(ostream& os) const {
	os << "break";
	if (value > 1) {
		os << " " << value;
	}
}

void Break::analyse(SemanticAnalyser* analyser, const Type& req_type) {

}

jit_value_t Break::compile_jit(Compiler& c, jit_function_t& F, Type type) const {

	jit_insn_branch(F, c.get_current_loop_end_label());

	return jit_value_create_nint_constant(F, jit_type_int, (long int) LSNull::null_var);
}
