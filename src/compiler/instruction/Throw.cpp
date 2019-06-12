#include "Throw.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/Function.hpp"
#include "../../vm/value/LSNull.hpp"

namespace ls {

Throw::Throw(std::shared_ptr<Token> token, Value* v) : token(token) {
	expression = v;
	throws = true;
}

Throw::~Throw() {
	delete expression;
}

void Throw::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "throw";
	if (expression != nullptr) {
		os << " ";
		expression->print(os, indent, debug);
	}
}

Location Throw::location() const {
	if (expression != nullptr) {
		return expression->location();
	} else {
		return token->location;
	}
}

void Throw::pre_analyze(SemanticAnalyzer* analyzer) {
	if (expression != nullptr) {
		expression->pre_analyze(analyzer);
	}
}

void Throw::analyze(SemanticAnalyzer* analyzer, const Type*) {
	if (expression != nullptr) {
		expression->analyze(analyzer);
	}
}

Compiler::value Throw::compile(Compiler& c) const {

	auto exception = c.new_integer((int) vm::Exception::EXCEPTION);
	if (expression != nullptr) {
		exception = expression->compile(c);
	}
	c.mark_offset(token->location.start.line);

	c.insn_throw(exception);

	return {};
}

Instruction* Throw::clone() const {
	auto ex = expression ? expression->clone() : nullptr;
	auto t = new Throw(token, ex);
	return t;
}

}
