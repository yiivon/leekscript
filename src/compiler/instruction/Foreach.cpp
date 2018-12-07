#include "../../compiler/instruction/Foreach.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../../type/RawType.hpp"

using namespace std;

namespace ls {

Foreach::Foreach() {
	key = nullptr;
	value = nullptr;
	body = nullptr;
	container = nullptr;
	key_var = nullptr;
	value_var = nullptr;
}

Foreach::~Foreach() {
	delete container;
	delete body;
}

void Foreach::print(ostream& os, int indent, bool debug) const {
	os << "for ";

	if (key != nullptr) {
		os << key->content;
		os << " : ";
	}
	os << value->content << " " << container->type.element();

	os << " in ";
	container->print(os, indent + 1, debug);

	os << " ";
	body->print(os, indent, debug);
}

Location Foreach::location() const {
	return {{0, 0, 0}, {0, 0, 0}};
}

void Foreach::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (req_type.is_array()) {
		type = req_type;
	} else {
		type = {};
	}
	analyser->enter_block();

	container->analyse(analyser);

	if (container->type._types.size() != 0 and not container->type.iterable()) {
		analyser->add_error({SemanticError::Type::VALUE_NOT_ITERABLE, container->location(), container->location(), {container->to_string(), container->type.to_string()}});
		return;
	}

	if (container->type.is_map()) {
		key_type = container->type.key();
		value_type = container->type.element();
	} else if (container->type.is_array() or container->type.is_interval() or container->type.is_set()) {
		key_type = Type::INTEGER;
		value_type = container->type.element();
	} else if (container->type.is_integer() || container->type.is_long()) {
		key_type = Type::INTEGER;
		value_type = Type::INTEGER;
	} else if (container->type.is_string()) {
		key_type = Type::INTEGER;
		value_type = Type::STRING;
	} else {
		key_type = Type::ANY;
		value_type = container->type.element();
	}

	if (key != nullptr) {
		key_var = analyser->add_var(key.get(), key_type, nullptr, nullptr);
	}
	value_var = analyser->add_var(value.get(), value_type, nullptr, nullptr);

	analyser->enter_loop();
	body->analyse(analyser);
	if (req_type.is_array()) {
		body->analyse(analyser);
		type = Type::array(body->type);
	}
	analyser->leave_loop();
	analyser->leave_block();
}

Compiler::value Foreach::compile(Compiler& c) const {

	c.enter_block(); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	Compiler::value output_v;
	if (type.is_array()) {
		output_v = c.new_array(type, {});
		c.insn_inc_refs(output_v);
		c.add_var("{output}", output_v); // Why create variable? in case of `break 2` the output must be deleted
	}

	auto container_v = container->compile(c);
	if (container->type.must_manage_memory()) {
		c.insn_inc_refs(container_v);
	}
	c.add_var("{array}", container_v);

	// Create variables
	auto value_var = c.create_and_add_var(value->content, container->type.element());
	c.insn_store(value_var, c.new_null());

	LLVMCompiler::value key_var;
	LLVMCompiler::value key_v;
	if (key) {
		key_v = c.new_integer(0);
		key_var = c.add_var(key->content, key_v);
	}

	auto it_label = c.insn_init_label("it");
	auto cond_label = c.insn_init_label("cond");
	auto end_label = c.insn_init_label("end");
	auto loop_label = c.insn_init_label("loop");

	c.enter_loop(&end_label, &it_label);

	auto it = c.iterator_begin(container_v);

	// For arrays, if begin iterator is 0, jump to end directly
	if (container->type.is_array()) {
		LLVMCompiler::value empty_array = {LLVMCompiler::builder.CreateICmpEQ(c.new_integer(0).v, c.to_int(it).v), Type::BOOLEAN};
		c.insn_if_new(empty_array, &end_label, &cond_label);
	} else {
		c.insn_branch(&cond_label);
	}

	// cond label:
	c.insn_label(&cond_label);
	// Condition to continue
	auto finished = c.iterator_end(container_v, it);
	c.insn_if_new(finished, &end_label, &loop_label);

	// loop label:
	c.insn_label(&loop_label);
	// Get Value
	c.insn_store(value_var, c.iterator_get(container->type, it, c.insn_load(value_var)));
	// Get Key
	if (key != nullptr) {
		c.insn_store(key_var, c.iterator_key(container_v, it, key_var));
	}
	// Body
	auto body_v = body->compile(c);
	if (output_v.v && body_v.v) {
		c.insn_push_array(output_v, body_v);
	}
	if (body_v.v) {
		c.insn_delete_temporary(body_v);
	}
	c.insn_branch(&it_label);

	// it++
	c.insn_label(&it_label);
	c.iterator_increment(container->type, it);
	// jump to cond
	c.insn_branch(&cond_label);

	c.leave_loop();

	// end label:
	c.insn_label(&end_label);

	auto return_v = c.clone(output_v); // otherwise it is delete by the c.leave_block
	c.leave_block(); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block
	return return_v;
}

Instruction* Foreach::clone() const {
	auto f = new Foreach();
	f->key = key;
	f->value = value;
	f->container = container->clone();
	f->body = (Block*) body->clone();
	return f;
}

}
