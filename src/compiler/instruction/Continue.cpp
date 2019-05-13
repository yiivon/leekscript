#include "Continue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/SemanticError.hpp"

namespace ls {

Continue::Continue() {
	deepness = 1;
}

Continue::~Continue() {}

void Continue::print(std::ostream& os, int, bool, bool) const {
	os << "continue";
	if (deepness > 1) {
		os << " " << deepness;
	}
}

Location Continue::location() const {
	return {{0, 0, 0}, {0, 0, 0}};
}

void Continue::analyze(SemanticAnalyzer* analyzer, const Type&) {

	// continue must be in a loop
	if (!analyzer->in_loop(deepness)) {
		analyzer->add_error({SemanticError::Type::CONTINUE_MUST_BE_IN_LOOP, {{0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}}}); // TODO location
	}
}

Compiler::value Continue::compile(Compiler& c) const {
	c.delete_variables_block(c.get_current_loop_blocks(deepness));
	c.insn_branch(c.get_current_loop_cond_label(deepness));
	c.insert_new_generation_block();
	return {};
}

Instruction* Continue::clone() const {
	auto c = new Continue();
	c->deepness = deepness;
	return c;
}

}
