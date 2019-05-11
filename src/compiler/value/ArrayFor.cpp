#include "ArrayFor.hpp"

namespace ls {

ArrayFor::~ArrayFor() {
	delete forr;
}

void ArrayFor::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "[";
	forr->print(os, indent, debug, condensed);
	os << "]";

	if (debug) {
		os << " " << type;
	}
}

Location ArrayFor::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void ArrayFor::analyse(SemanticAnalyser* analyser) {
	forr->analyse(analyser, Type::array());
	type = forr->type;
	throws = forr->throws;
}

Compiler::value ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}

Value* ArrayFor::clone() const {
	auto af = new ArrayFor();
	af->forr = forr->clone();
	return af;
}

}
