#include "../../compiler/instruction/Throw.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Throw::Throw(Token* token, Value* v) {
	this->token = token;
	expression = v;
}

Throw::~Throw() {
	delete expression;
}

void Throw::print(ostream& os, int indent, bool debug) const {
	os << "throw ";
	if (expression != nullptr) {
		expression->print(os, indent, debug);
	}
}

void Throw::analyse(SemanticAnalyser* analyser, const Type&) {
	if (expression != nullptr) {
		expression->analyse(analyser, Type::UNKNOWN);
	}
}

Compiler::value Throw::compile(Compiler& c) const {

	if (expression != nullptr) {
		auto v = expression->compile(c);

		/*
		auto stack = jit_insn_get_call_stack(c.F);
		Compiler::value stack_v = {stack, Type::POINTER};
		c.insn_call(Type::VOID, {stack_v}, +[](void* stack) {
			std::cout << "stack before throw: " << stack << std::endl;
		});
		*/

		auto line = c.new_integer(token->line);

		auto ex = c.insn_call(Type::POINTER, {v, line}, +[](int obj, int line) {
			auto ex = new VM::ExceptionObj((VM::Exception) obj);
			ex->lines.push_back(line);
			return ex;
		});
		jit_insn_throw(c.F, ex.v);

		/*
		c.insn_call(Type::VOID, {}, +[]() {
			auto ex = new VM::ExceptionObj();
			ex->obj = LSNull::get();
			jit_exception_throw(ex);
		});
		*/
	}

	return {nullptr, Type::UNKNOWN};
}

}
