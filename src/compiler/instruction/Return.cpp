#include "../../compiler/instruction/Return.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Return::Return(Value* v) {
	expression = v;
	returning = true;
	may_return = true;
}

Return::~Return() {
	delete expression;
}

void Return::print(ostream& os, int indent, bool debug) const {
	os << "return ";
	if (expression != nullptr) {
		expression->print(os, indent, debug);
	}
}

void Return::analyse(SemanticAnalyser* analyser, const Type&) {
	if (expression != nullptr) {
		expression->analyse(analyser);
		return_type = expression->type;
	}
}

Location Return::location() const {
	return expression->location();
}

Compiler::value Return::compile(Compiler& c) const {
	if (expression != nullptr) {
		auto return_type = c.fun->getReturnType().fold();
		auto v = c.insn_convert(expression->compile(c), return_type);
		c.assert_value_ok(v);
		auto r = c.insn_move(v);
		c.delete_function_variables();
		c.insn_return(r);
	} else {
		c.insn_return_void();
	}
	return {};
}

Instruction* Return::clone() const {
	auto ex = expression ? expression->clone() : nullptr;
	auto r = new Return(ex);
	return r;
}

}
