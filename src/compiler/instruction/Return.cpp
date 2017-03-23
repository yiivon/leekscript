#include "../../compiler/instruction/Return.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Return::Return(Value* v) {
	expression = v;
	function = nullptr;
	in_function = false;
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

	Function* f = analyser->current_function();

	if (expression != nullptr) {
		Type required_type = Type::UNKNOWN;
		if (f->type.getReturnType() != Type::UNKNOWN) {
			required_type = f->type.getReturnType();
		}
		expression->analyse(analyser, required_type);
		f->body->types.add(expression->type);
	}
	function = f;
	in_function = true;
	type = Type::VOID;
}

Compiler::value Return::compile(Compiler& c) const {

	if (expression != nullptr) {

		auto v = expression->compile(c);

		auto r = c.insn_move(v);
		c.delete_variables_block(c.get_current_function_blocks());
		jit_insn_return(c.F, r.v);
	}

	jit_insn_return(c.F, c.new_integer(0).v);
	return {nullptr, Type::UNKNOWN};
}

}
