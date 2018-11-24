#include "AbsoluteValue.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
	type = Type::INTEGER;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << "|";
	expression->print(os, debug);
	os << "|";
	if (debug) {
		os << " " << type;
	}
}

Location AbsoluteValue::location() const {
	return {open_pipe->location.start, close_pipe->location.end};
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	expression->analyse(analyser);
	constant = expression->constant;
}

Compiler::value AbsoluteValue::compile(Compiler& c) const {
	auto ex = c.insn_to_pointer(expression->compile(c));
	c.mark_offset(location().start.line);
	auto abso = c.insn_call(Type::INTEGER, {ex}, (void*) +[](LSValue* v) {
		return v->abso();
	});
	c.insn_delete_temporary(ex);
	return abso;
}

Value* AbsoluteValue::clone() const {
	auto abs = new AbsoluteValue();
	abs->expression = expression;
	abs->open_pipe = open_pipe;
	abs->close_pipe = close_pipe;
	return abs;
}

}
