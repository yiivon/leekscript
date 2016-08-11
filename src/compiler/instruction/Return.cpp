#include "../../compiler/instruction/Return.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Return::Return() : Return(nullptr) {}

Return::Return(Value* v) {
	expression = v;
	function = nullptr;
	in_function = false;
	can_return = true;
}

Return::~Return() {
	delete expression;
}

void Return::print(ostream& os, int indent, bool debug) const {
	os << tabs(indent) << "return ";
	expression->print(os, indent, debug);
}

void Return::analyse(SemanticAnalyser* analyser, const Type& ) {

	Function* f = analyser->current_function();
	expression->analyse(analyser, Type::POINTER);
	if (f != nullptr) {
		f->type.setReturnType(expression->type);
		function = f;
		in_function = true;
	} else {
		function = nullptr;
		in_function = false;
	}
	type = Type::VOID;
}

jit_value_t Return::compile(Compiler& c) const {

	jit_value_t v = expression->compile(c);
	jit_insn_return(c.F, v);
	return nullptr;
}

}
