#include "Foreach.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

Foreach::Foreach() {
	wrapper_block = std::make_unique<Block>();
	key_var = nullptr;
	value_var = nullptr;
}

void Foreach::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "for ";

	if (key != nullptr) {
		os << key->content << " " << container->type->key();
		os << " : ";
	}
	os << value->content << " " << container->type->element();

	os << " in ";
	container->print(os, indent + 1, debug);

	os << " ";
	body->print(os, indent, debug);
	os << " ";
	body2->print(os, indent, debug);
	for (const auto& assignment : assignments) {
		os << std::endl << tabs(indent) << assignment.first << " = " << assignment.second;
	}
}

Location Foreach::location() const {
	return {nullptr, {0, 0, 0}, {0, 0, 0}};
}

void Foreach::pre_analyze(SemanticAnalyzer* analyzer) {
	analyzer->enter_block(wrapper_block.get());
	container->pre_analyze(analyzer);
	if (key != nullptr) {
		key_var = analyzer->add_var(key, Type::void_, nullptr);
	}
	value_var = analyzer->add_var(value, Type::void_, nullptr);
	body->is_loop_body = true;
	body->pre_analyze(analyzer);
	body2 = std::move(unique_static_cast<Block>(body->clone()));
	body2->is_loop_body = true;
	body2->variables = body->variables;
	body2->is_loop = true;
	body2->pre_analyze(analyzer);
	analyzer->leave_block();

	for (const auto& variable : body->variables) {
		// std::cout << "Foreach assignment " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
		if (variable.second->parent) {
			auto new_var = analyzer->update_var(variable.second->parent);
			variable.second->assignment = true;
			assignments.push_back({ new_var, variable.second });
		}
	}
}

void Foreach::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {

	if (req_type->is_array()) {
		type = req_type;
	} else {
		type = Type::void_;
		body->is_void = true;
		body2->is_void = true;
	}
	analyzer->enter_block(wrapper_block.get());

	container->analyze(analyzer);
	throws = container->throws;

	if (not container->type->is_void() and not container->type->iterable() and not container->type->is_any()) {
		analyzer->add_error({Error::Type::VALUE_NOT_ITERABLE, container->location(), container->location(), {container->to_string(), container->type->to_string()}});
		return;
	}

	key_type = container->type->key();
	value_type = container->type->element();
	if (key != nullptr) {
		key_var->type = key_type;
	}
	value_var->type = value_type;

	analyzer->enter_loop();
	body->analyze(analyzer);
	throws |= body->throws;
	if (req_type->is_array()) {
		type = Type::tmp_array(body->type);
	}
	body2->analyze(analyzer);
	analyzer->leave_loop();
	analyzer->leave_block();

	for (const auto& assignment : assignments) {
		assignment.first->type = assignment.second->type;
	}
}

Compiler::value Foreach::compile(Compiler& c) const {
	auto container_v = container->compile(c);
	value_var->create_entry(c);
	if (key_var) key_var->create_entry(c);
	auto result = c.insn_foreach(container_v, type->element(), value_var, key_var, [&](Compiler::value value, Compiler::value key) {
		return body->compile(c);
	}, false, [&](Compiler::value value, Compiler::value key) {
		return body2->compile(c);
	});
	for (const auto& assignment : assignments) {
		// std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
		assignment.first->val = c.create_entry(assignment.first->name, assignment.first->type);
		c.insn_store(assignment.first->val, c.insn_load(assignment.second->val));
	}
	return result;
}

std::unique_ptr<Instruction> Foreach::clone() const {
	auto f = std::make_unique<Foreach>();
	f->key = key;
	f->value = value;
	f->container = container->clone();
	f->body = unique_static_cast<Block>(body->clone());
	return f;
}

}
