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

void Return::analyze(SemanticAnalyzer* analyzer, const Type&) {
	if (expression != nullptr) {
		expression->analyze(analyzer);
		return_type = expression->type;
	}
}

Location Return::location() const {
	return expression->location();
}

Compiler::value Return::compile(Compiler& c) const {
	if (expression != nullptr) {
		auto r = expression->compile(c);
		r = c.insn_move(r);
		c.delete_function_variables();
		c.fun->compile_return(c, r);
	} else {
		c.delete_function_variables();
		c.fun->compile_return(c, {});
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
