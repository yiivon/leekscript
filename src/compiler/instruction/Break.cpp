#include "../../compiler/instruction/Break.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"

using namespace std;

namespace ls {

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

void Break::analyse(SemanticAnalyser* analyser, const Type&) {

	// break must be in a loop
	if (!analyser->in_loop()) {
		throw SemanticException(SemanticException::Type::BREAK_MUST_BE_IN_LOOP, 0);
	}
}

jit_value_t Break::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_insn_branch(F, c.get_current_loop_end_label());

	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}

}
