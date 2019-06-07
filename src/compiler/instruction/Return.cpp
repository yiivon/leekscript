#include "Return.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/Function.hpp"

namespace ls {

Return::Return(Value* v) {
	expression = v;
	returning = true;
	may_return = true;
}

Return::~Return() {
	delete expression;
}

void Return::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "return ";
	if (expression != nullptr) {
		expression->print(os, indent, debug);
	}
}

void Return::analyze(SemanticAnalyzer* analyzer, const Type*) {
	if (expression != nullptr) {
		expression->analyze(analyzer);
		return_type = expression->type;
		if (return_type->is_mpz_ptr()) return_type = Type::tmp_mpz;
		throws = expression->throws;
	}
}

Location Return::location() const {
	return expression->location();
}

Compiler::value Return::compile(Compiler& c) const {
	if (expression != nullptr) {
		auto r = expression->compile(c);
		if (r.t == Type::mpz_ptr) {
			r = c.insn_load(c.insn_clone_mpz(r));
		} else if (r.t == Type::tmp_mpz_ptr) {
			r = c.insn_load(r);
		} else {
			r = c.insn_move(r);
		}
		c.fun->compile_return(c, r, true);
	} else {
		c.fun->compile_return(c, {}, true);
	}
	c.insert_new_generation_block();
	return {};
}

Instruction* Return::clone() const {
	auto ex = expression ? expression->clone() : nullptr;
	auto r = new Return(ex);
	return r;
}

}
