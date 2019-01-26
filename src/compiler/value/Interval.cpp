#include "Interval.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include <math.h>

using namespace std;

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

void Interval::analyse(SemanticAnalyser* analyser) {
	constant = true;
	type = Type::interval();
	type.temporary = true;
	start->analyse(analyser);
	end->analyse(analyser);
}

Compiler::value Interval::compile(Compiler& c) const {
	auto a = c.to_int(start->compile(c));
	auto b = c.to_int(end->compile(c));
	return c.insn_call(Type::tmp_interval(), {a, b}, +[](int a, int b) {
		// TODO a better constructor?
		LSInterval* interval = new LSInterval();
		interval->a = a;
		interval->b = b;
		return interval;
	});
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
