#include "../../compiler/instruction/Throw.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Throw::Throw(Token* token, Value* v) {
	this->token.reset(token);
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

void Throw::analyse(SemanticAnalyser* analyser, const Type&) {
	if (expression != nullptr) {
		expression->analyse(analyser, Type::UNKNOWN);
	}
}

Compiler::value Throw::compile(Compiler& c) const {

	auto exception = c.new_integer((int) VM::Exception::EXCEPTION);
	if (expression != nullptr) {
		exception = expression->compile(c);
	}

	jit_insn_mark_offset(c.F, token->line);

	auto ex = c.insn_call(Type::POINTER, {exception}, &VM::get_exception_object<0>);

	jit_insn_throw(c.F, ex.v);

	return {nullptr, Type::UNKNOWN};
}

}
