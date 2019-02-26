#include "For.hpp"
#include "../instruction/Return.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"

namespace ls {

For::For() {}

For::~For() {
	delete condition;
	delete body;
	for (Instruction* ins : inits) {
		delete ins;
	}
	for (Instruction* ins : increments) {
		delete ins;
	}
}

void For::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "for";
	for (auto ins : inits) {
		os << " ";
		ins->print(os, indent + 1, debug, condensed);
	}
	os << "; ";
	if (condition != nullptr) {
		condition->print(os, indent + 1, debug);
	}
	os << ";";
	for (auto ins : increments) {
		os << " ";
		ins->print(os, indent + 1, debug, condensed);
	}
	os << " ";
	body->print(os, indent, debug);
}

Location For::location() const {
	return {{0, 0, 0}, {0, 0, 0}};
}

void For::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	// std::cout << "For::analyse() " << is_void << std::endl;

	if (req_type.is_array()) {
		type = req_type;
	} else {
		type = {};
		body->is_void = true;
	}

	analyser->enter_block();

	// Init
	for (Instruction* ins : inits) {
		ins->analyse(analyser);
		if (ins->may_return) {
			returning = ins->returning;
			may_return = ins->may_return;
			return_type += ins->return_type;
		}
		if (ins->returning) {
			analyser->leave_block();
			return;
		}
	}

	// Condition
	if (condition != nullptr) {
		condition->analyse(analyser);
	}

	// Body
	analyser->enter_loop();
	body->analyse(analyser);
	if (body->returning) returning = true;
	if (body->may_return) may_return = true;
	return_type += body->return_type;
	if (req_type.is_array()) {
		type = Type::array(body->type);
	}
	analyser->leave_loop();

	// Increment
	analyser->enter_block();
	for (Instruction* ins : increments) {
		ins->is_void = true;
		ins->analyse(analyser, {});
		if (ins->may_return) {
			returning = ins->returning;
			may_return = ins->may_return;
			return_type += ins->return_type;
		}
		if (ins->returning) {
			break;
		}
	}
	analyser->leave_block();

	analyser->leave_block();
}

Compiler::value For::compile(Compiler& c) const {

	c.enter_block(); // { for init ; cond ; inc { body } }<-- this block

	Compiler::value output_v;
	if (type.is_array()) {
		output_v = c.new_array(type.element(), {});
		c.insn_inc_refs(output_v);
		c.add_var("{output}", output_v); // Why create variable ? in case of `break 2` the output must be deleted
	}

	auto cond_label = c.insn_init_label("cond");
	auto end_label = c.insn_init_label("afterloop");
	auto loop_label = c.insn_init_label("loop");
	auto inc_label = c.insn_init_label("inc");

	// Init
	for (Instruction* ins : inits) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			auto return_v = c.clone(output_v);
			c.leave_block();
			return return_v;
		}
	}
	c.insn_branch(&cond_label);

	// Cond
	c.insn_label(&cond_label);
	c.inc_ops(1);
	if (condition != nullptr) {
		auto condition_v = condition->compile(c);
		condition->compile_end(c);
		auto bool_v = c.insn_to_bool(condition_v);
		c.insn_delete_temporary(condition_v);
		c.insn_if_new(bool_v, &loop_label, &end_label);
	} else {
		c.insn_branch(&loop_label);
	}

	// Body
	c.insn_label(&loop_label);
	c.enter_loop(&end_label, &inc_label);
	auto body_v = body->compile(c);
	if (output_v.v && body_v.v) {
		// transfer the ownership of the temporary variable `body_v`
		c.insn_push_array(output_v, body_v);
	}
	if (body_v.v) {
		c.insn_delete_temporary(body_v);
	}
	c.leave_loop();
	c.insn_branch(&inc_label);

	// Inc
	c.insn_label(&inc_label);
	c.enter_block();
	for (auto& ins : increments) {
		auto r = ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			return r;
		}
	}
	c.leave_block();
	c.insn_branch(&cond_label);

	// End
	c.insn_label(&end_label);
	auto return_v = c.clone(output_v);
	c.leave_block();
	return return_v;
}

Instruction* For::clone() const {
	auto f = new For();
	for (const auto& i : inits) {
		f->inits.push_back(i->clone());
	}
	f->condition = condition->clone();
	for (const auto& i : increments) {
		f->increments.push_back(i->clone());
	}
	f->body = (Block*) body->clone();
	return f;
}

}
