#include "../../compiler/instruction/Continue.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"

namespace ls {

Continue::Continue() {
	value = 1;
	can_return = false;
}

Continue::~Continue() {}

void Continue::print(std::ostream& os) const {
	os << "continue";
	if (value > 1) {
		os << " " << value;
	}
}

void Continue::analyse(SemanticAnalyser* analyser, const Type&) {

	// continue must be in a loop
	if (!analyser->in_loop()) {
		analyser->add_error({SemanticException::Type::CONTINUE_MUST_BE_IN_LOOP, 0});
	}
}

jit_value_t Continue::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_insn_branch(F, c.get_current_loop_cond_label());

	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}

}
