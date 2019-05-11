#include "AbsoluteValue.hpp"
#include "../../vm/LSValue.hpp"

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
	type = Type::integer();
	throws = true;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << "|";
	expression->print(os, 0, debug);
	os << "|";
	if (debug) {
		os << " " << type;
	}
}

Location AbsoluteValue::location() const {
	return {open_pipe->location.start, close_pipe->location.end};
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser) {
	expression->analyse(analyser);
	constant = expression->constant;
}

Compiler::value AbsoluteValue::compile(Compiler& c) const {
	auto ex = c.insn_to_any(expression->compile(c));
	c.mark_offset(location().start.line);
	auto abso = c.insn_invoke(Type::integer(), {ex}, "Value.absolute");
	c.insn_delete_temporary(ex);
	return abso;
}

Value* AbsoluteValue::clone() const {
	auto abs = new AbsoluteValue();
	abs->expression = expression->clone();
	abs->open_pipe = open_pipe;
	abs->close_pipe = close_pipe;
	return abs;
}

}
