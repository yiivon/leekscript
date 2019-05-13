#include "Interval.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include <math.h>

namespace ls {

Interval::Interval() {
	type = Type::interval();
}

Interval::~Interval() {
	delete start;
	delete end;
}

void Interval::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "[";
	start->print(os, indent, debug);
	os << "..";
	end->print(os, indent, debug);
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

Location Interval::location() const {
	return {opening_bracket->location.start, closing_bracket->location.end};
}

void Interval::analyze(SemanticAnalyzer* analyzer) {
	constant = true;
	type = Type::interval();
	type.temporary = true;
	start->analyze(analyzer);
	end->analyze(analyzer);
}

Compiler::value Interval::compile(Compiler& c) const {
	auto a = start->compile(c);
	auto b = end->compile(c);
	auto int_a = c.to_int(a);
	auto int_b = c.to_int(b);
	auto interval = c.insn_call(Type::tmp_interval(), {int_a, int_b}, "Interval.new");
	c.insn_delete_temporary(a);
	c.insn_delete_temporary(b);
	return interval;
}

Value* Interval::clone() const {
	auto interval = new Interval();
	interval->opening_bracket = opening_bracket;
	interval->closing_bracket = closing_bracket;
	interval->start = start->clone();
	interval->end = end->clone();
	return interval;
}

}
