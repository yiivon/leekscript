#include "../../compiler/instruction/While.hpp"

#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

While::While() {
	condition = nullptr;
	body = nullptr;
}

While::~While() {
	delete condition;
	delete body;
}

void While::print(ostream& os, int indent, bool debug) const {
	os << "while ";
	condition->print(os, indent + 1, debug);
	os << " ";
	body->print(os, indent, debug);
}

Location While::location() const {
	return token->location;
}

void While::analyse(SemanticAnalyser* analyser, const Type&) {

	condition->analyse(analyser);
	analyser->enter_loop();
	body->analyse(analyser);
	analyser->leave_loop();
}

Compiler::value While::compile(Compiler& c) const {

	auto cond_label = c.insn_init_label("cond");
	auto end_label = c.insn_init_label("afterloop");
	auto loop_label = c.insn_init_label("loop");

	c.insn_branch(&cond_label);
	c.insn_label(&cond_label);
	auto cond = condition->compile(c);
	condition->compile_end(c);
	if (condition->type.nature == Nature::POINTER) {
		auto cond_bool = c.insn_to_bool(cond);
		c.insn_delete_temporary(cond);
		cond = cond_bool;
	}
	c.insn_if_new(cond, &loop_label, &end_label);

	c.insn_label(&loop_label);
	c.inc_ops(1);
	c.enter_loop(&end_label, &loop_label);
	body->compile(c);
	c.leave_loop();
	c.insn_branch(&cond_label);

	c.insn_label(&end_label);
	return {nullptr, Type::VOID};
}

Instruction* While::clone() const {
	auto w = new While();
	w->token = token;
	w->condition = condition->clone();
	w->body = (Block*) body->clone();
	return w;
}

}
