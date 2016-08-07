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

void Break::print(ostream& os, int indent, bool) const {
	os << tabs(indent) << "break";
	if (value > 1) {
		os << " " << value;
	}
}

void Break::analyse(SemanticAnalyser* analyser, const Type&) {

	// break must be in a loop
	if (!analyser->in_loop()) {
		analyser->add_error({SemanticException::Type::BREAK_MUST_BE_IN_LOOP, 0});
	}
}

jit_value_t Break::compile(Compiler& c) const {

	jit_insn_branch(c.F, c.get_current_loop_end_label());

	return nullptr;
}

}
