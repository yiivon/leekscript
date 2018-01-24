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

	condition->analyse(analyser, Type::UNKNOWN);
	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();
}

Compiler::value While::compile(Compiler& c) const {

	Compiler::label label_cond;
	Compiler::label label_end;

	c.mark_offset(location().start.line);

	// condition
	c.insn_label(&label_cond);
	c.inc_ops(1);
	auto cond = condition->compile(c);
	condition->compile_end(c);
	if (condition->type.nature == Nature::POINTER) {
		auto cond_bool = c.insn_to_bool(cond);
		c.insn_delete_temporary(cond);
		c.insn_branch_if_not(cond_bool, &label_end);
	} else {
		c.insn_branch_if_not(cond, &label_end);
	}

	// body
	c.enter_loop(&label_end, &label_cond);
	body->compile(c);
	c.leave_loop();

	// jump to cond
	c.insn_branch(&label_cond);

	// end label:
	c.insn_label(&label_end);

	return {nullptr, Type::UNKNOWN};
}

Instruction* While::clone() const {
	auto w = new While();
	w->token = token;
	w->condition = condition->clone();
	w->body = (Block*) body->clone();
	return w;
}

}
