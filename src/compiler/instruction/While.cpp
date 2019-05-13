#include "While.hpp"
#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

While::While() {
	condition = nullptr;
	body = nullptr;
}

While::~While() {
	delete condition;
	delete body;
}

void While::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "while ";
	condition->print(os, indent + 1, debug);
	os << " ";
	body->print(os, indent, debug);
}

Location While::location() const {
	return token->location;
}

void While::analyze(SemanticAnalyzer* analyzer, const Type&) {
	condition->analyze(analyzer);
	throws |= condition->throws;

	analyzer->enter_loop();
	body->is_void = true;
	body->analyze(analyzer);
	analyzer->leave_loop();
	
	throws |= body->throws;
	if (body->may_return) {
		may_return = body->may_return;
		returning = body->returning;
		return_type = body->return_type;
	}
}

Compiler::value While::compile(Compiler& c) const {
	auto cond_label = c.insn_init_label("cond");
	auto end_label = c.insn_init_label("afterloop");
	auto loop_label = c.insn_init_label("loop");
	c.insn_branch(&cond_label);
	c.insn_label(&cond_label);
	auto cond = condition->compile(c);
	condition->compile_end(c);
	auto cond_bool = c.insn_to_bool(cond);
	c.insn_delete_temporary(cond);
	c.insn_if_new(cond_bool, &loop_label, &end_label);
	c.insn_label(&loop_label);
	c.inc_ops(1);
	c.enter_loop(&end_label, &cond_label);
	auto body_v = body->compile(c);
	if (body_v.v) {
		c.insn_delete_temporary(body_v);
	}
	c.leave_loop();
	c.insn_branch(&cond_label);
	c.insn_label(&end_label);
	return {};
}

Instruction* While::clone() const {
	auto w = new While();
	w->token = token;
	w->condition = condition->clone();
	w->body = (Block*) body->clone();
	return w;
}

}
