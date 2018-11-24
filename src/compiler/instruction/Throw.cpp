#include "../../compiler/instruction/Throw.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Throw::Throw(std::shared_ptr<Token> token, Value* v) : token(token) {
	expression = v;
}

Throw::~Throw() {
	delete expression;
}

void Throw::print(ostream& os, int indent, bool debug) const {
	os << "throw";
	if (expression != nullptr) {
		os << " ";
		expression->print(os, indent, debug);
	}
}

Location Throw::location() const {
	return expression->location();
}

void Throw::analyse(SemanticAnalyser* analyser, const Type&) {
	if (expression != nullptr) {
		expression->analyse(analyser);
	}
}

Compiler::value Throw::compile(Compiler& c) const {

	auto exception = c.new_integer((int) vm::Exception::EXCEPTION);
	if (expression != nullptr) {
		exception = expression->compile(c);
	}
	c.mark_offset(token->location.start.line);

	c.insn_throw(exception);

	return {nullptr, Type::ANY};
}

Instruction* Throw::clone() const {
	auto ex = expression ? expression->clone() : nullptr;
	auto t = new Throw(token, ex);
	return t;
}

}
