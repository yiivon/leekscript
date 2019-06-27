#include "For.hpp"
#include "../instruction/Return.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../value/Value.hpp"

namespace ls {

void For::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "for";
	for (const auto& ins : init->instructions) {
		os << " ";
		ins->print(os, indent + 1, debug, condensed);
	}
	os << "; ";
	if (condition != nullptr) {
		condition->print(os, indent + 1, debug);
	}
	os << ";";
	for (const auto& ins : increment->instructions) {
		os << " ";
		ins->print(os, indent + 1, debug, condensed);
	}
	os << " ";
	body->print(os, indent, debug);
	if (body2) {
		os << " ";
		body2->print(os, indent, debug);
	}
	for (const auto& assignment : assignments) {
		os << std::endl << tabs(indent) << assignment.first << " = " << assignment.second;
	}
}

Location For::location() const {
	return {nullptr, {0, 0, 0}, {0, 0, 0}};
}

void For::pre_analyze(SemanticAnalyzer* analyzer) {
	init->setup_branch(analyzer);
	analyzer->enter_block(init.get());
	for (const auto& ins : init->instructions) {
		ins->pre_analyze(analyzer);
	}
	if (condition != nullptr) {
		condition->pre_analyze(analyzer);
	}
	body->is_loop_body = true;
	body->pre_analyze(analyzer);

	if (body->variables.size()) {
		body2 = std::move(unique_static_cast<Block>(body->clone()));
		body2->is_loop_body = true;
		body2->is_loop = true;
		for (const auto& variable : body->variables) {
			if (variable.second->parent and variable.second->parent->block != body.get()) {
				body2->variables.insert({ variable.first, variable.second });
			}
		}
		body2->pre_analyze(analyzer);
	}

	analyzer->enter_block(increment.get());
	for (const auto& ins : increment->instructions) {
		ins->pre_analyze(analyzer);
	}
	analyzer->leave_block();
	analyzer->leave_block();

	for (const auto& variable : init->variables) {
		// std::cout << "For update_var " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
		if (variable.second->parent and variable.second->block->branch == analyzer->current_block()->branch) {
			auto new_var = analyzer->update_var(variable.second->parent);
			assignments.push_back({ new_var, variable.second });
		}
	}
	for (const auto& variable : body->variables) {
		// std::cout << "Foreach assignment " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
		if (variable.second->parent) {
			auto new_var = analyzer->update_var(variable.second->parent);
			variable.second->assignment = true;
			assignments.push_back({ new_var, variable.second });
		}
	}
}

void For::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {
	// std::cout << "For::analyze() " << is_void << std::endl;

	if (req_type->is_array()) {
		type = req_type;
	} else {
		type = Type::void_;
		body->is_void = true;
		if (body2) body2->is_void = true;
	}

	analyzer->enter_block(init.get());
	throws = false;

	// Init
	for (const auto& ins : init->instructions) {
		ins->analyze(analyzer);
		throws |= ins->throws;
		if (ins->may_return) {
			returning = ins->returning;
			may_return = ins->may_return;
			return_type = return_type->operator + (ins->return_type);
		}
		if (ins->returning) {
			analyzer->leave_block();
			return;
		}
	}

	// Condition
	if (condition != nullptr) {
		condition->analyze(analyzer);
		throws |= condition->throws;
	}

	// Body
	analyzer->enter_loop();
	body->analyze(analyzer);
	throws |= body->throws;
	if (body->returning) returning = true;
	if (body->may_return) may_return = true;
	return_type = return_type->operator + (body->return_type);
	if (req_type->is_array()) {
		type = Type::array(body->type);
	}
	if (body2) {
		body2->analyze(analyzer);
	}
	analyzer->leave_loop();

	// Increment
	analyzer->enter_block(increment.get());
	for (const auto& ins : increment->instructions) {
		ins->is_void = true;
		ins->analyze(analyzer);
		throws |= ins->throws;
		if (ins->may_return) {
			returning = ins->returning;
			may_return = ins->may_return;
			return_type = return_type->operator + (ins->return_type);
		}
		if (ins->returning) {
			break;
		}
	}
	analyzer->leave_block();

	analyzer->leave_block();

	for (const auto& assignment : assignments) {
		assignment.first->type = assignment.second->type;
	}
}

Compiler::value For::compile(Compiler& c) const {

	c.enter_block(init.get()); // { for init ; cond ; inc { body } }<-- this block
	c.mark_offset(token->location.start.line);

	Compiler::value output_v;
	if (type->is_array()) {
		output_v = c.new_array(type->element(), {});
		c.insn_inc_refs(output_v);
		c.add_temporary_value(output_v); // Why create variable ? in case of `break 2` the output must be deleted
	}

	auto cond_label = c.insn_init_label("cond");
	auto loop_label = c.insn_init_label("loop");
	auto inc_label = c.insn_init_label("inc");
	auto end_label = c.insn_init_label("end");
	Compiler::label cond2_label;
	Compiler::label loop2_label;
	Compiler::label inc2_label;
	if (body2) {
		cond2_label = c.insn_init_label("cond2");
		loop2_label = c.insn_init_label("loop2");
		inc2_label = c.insn_init_label("inc2");
	}

	// Init
	for (const auto& ins : init->instructions) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins.get())) {
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
	c.leave_loop();
	c.insn_branch(&inc_label);

	// Inc
	c.insn_label(&inc_label);
	c.enter_block(increment.get());
	for (auto& ins : increment->instructions) {
		auto r = ins->compile(c);
		if (dynamic_cast<Return*>(ins.get())) {
			return r;
		}
	}
	c.leave_block();
	c.insn_branch(body2 ? &cond2_label : &cond_label);

	// Cond 2
	if (body2) {
		c.insn_label(&cond2_label);
		c.inc_ops(1);
		if (condition != nullptr) {
			auto condition_v = condition->compile(c);
			condition->compile_end(c);
			auto bool_v = c.insn_to_bool(condition_v);
			c.insn_delete_temporary(condition_v);
			c.insn_if_new(bool_v, &loop2_label, &end_label);
		} else {
			c.insn_branch(&loop2_label);
		}
		// Body 2
		c.insn_label(&loop2_label);
		c.enter_loop(&end_label, &inc2_label);
		auto body2_v = body2->compile(c);
		if (output_v.v && body2_v.v) {
			// transfer the ownership of the temporary variable `body_v`
			c.insn_push_array(output_v, body2_v);
		}
		c.leave_loop();
		c.insn_branch(&inc2_label);
		// Inc 2
		c.insn_label(&inc2_label);
		c.enter_block(increment.get());
		for (auto& ins : increment->instructions) {
			auto r = ins->compile(c);
			if (dynamic_cast<Return*>(ins.get())) {
				return r;
			}
		}
		c.leave_block();
		c.insn_branch(&cond2_label);
	}

	// End
	c.insn_label(&end_label);
	auto return_v = c.clone(output_v);


	c.leave_block();

	for (const auto& assignment : assignments) {
		// std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
		assignment.first->val = c.create_entry(assignment.first->name, assignment.first->type);
		c.insn_store(assignment.first->val, c.insn_load(assignment.second->val));
	}
	return return_v;
}

std::unique_ptr<Instruction> For::clone() const {
	auto f = std::make_unique<For>();
	f->token = token;
	f->init = unique_static_cast<Block>(init->clone());
	f->condition = condition->clone();
	f->increment = unique_static_cast<Block>(increment->clone());
	f->body = unique_static_cast<Block>(body->clone());
	return f;
}

}
