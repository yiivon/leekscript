#include "../../compiler/instruction/Foreach.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSSet.hpp"

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
	os << value->content;

	os << " in ";
	container->print(os, indent + 1, debug);

	os << " ";
	body->print(os, indent, debug);
}

void Foreach::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (req_type.raw_type == RawType::ARRAY && req_type.nature == Nature::POINTER) {
		type = req_type;
	} else {
		type = Type::VOID;
	}

	analyser->enter_block();

	container->analyse(analyser, Type::UNKNOWN);

	if (container->type != Type::UNKNOWN and not container->type.iterable()) {
		analyser->add_error({SemanticError::Type::VALUE_NOT_ITERABLE, container->line(), {container->to_string(), container->type.to_string()}});
	}

	if (container->type.raw_type == RawType::MAP) {
		key_type = container->type.getKeyType();
		value_type = container->type.getElementType();
	} else if (container->type.raw_type == RawType::ARRAY or container->type.raw_type == RawType::INTERVAL or container->type.raw_type == RawType::SET) {
		key_type = Type::INTEGER;
		value_type = container->type.getElementType();
	} else if (container->type.raw_type == RawType::INTEGER ||
			   container->type.raw_type == RawType::LONG) {
		key_type = Type::INTEGER;
		value_type = Type::INTEGER;
	} else if (container->type.raw_type == RawType::STRING) {
		key_type = Type::INTEGER;
		value_type = Type::STRING;
	} else {
		key_type = Type::POINTER;
		value_type = container->type.getElementType();
	}

	if (key != nullptr) {
		key_var = analyser->add_var(key, key_type, nullptr, nullptr);
	}
	value_var = analyser->add_var(value, value_type, nullptr, nullptr);

	analyser->enter_loop();
	if (type == Type::VOID) {
		body->analyse(analyser, Type::VOID);
	} else {
		body->analyse(analyser, type.getElementType());
		type.setElementType(body->type);
	}
	analyser->leave_loop();
	analyser->leave_block();
}

Compiler::value Foreach::compile(Compiler& c) const {

	c.enter_block(); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	jit_value_t output_v = nullptr;
	if (type.raw_type == RawType::ARRAY && type.nature == Nature::POINTER) {
		output_v = VM::create_array(c.F, type.getElementType());
		VM::inc_refs(c.F, output_v);
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	auto container_v = container->compile(c);

	if (container->type.must_manage_memory()) {
		VM::inc_refs(c.F, container_v.v);
	}
	c.add_var("{array}", container_v.v, container->type, false);

	// Create variables
	jit_type_t jit_value_type = VM::get_jit_type(value_type);
	jit_value_t value_v = jit_value_create(c.F, jit_value_type);
	jit_type_t jit_key_type = VM::get_jit_type(key_type);
	jit_value_t key_v = key ? jit_value_create(c.F, jit_key_type) : nullptr;

	c.add_var(value->content, value_v, value_type, true);
	if (key) c.add_var(key->content, key_v, key_type, true);

	jit_label_t label_end = jit_label_undefined;
	jit_label_t label_it = jit_label_undefined;
	c.enter_loop(&label_end, &label_it);

	jit_label_t label_cond = jit_label_undefined;

	auto it = c.iterator_begin(container_v);

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// Condition to continue
	auto finished = c.iterator_end(container_v, it);
	jit_insn_branch_if(c.F, finished.v, &label_end);

	// Get Value
	jit_insn_store(c.F, value_v, c.iterator_get(it).v);

	// Get Key
	if (key != nullptr) {
		jit_insn_store(c.F, key_v, c.iterator_key(container_v, it).v);
	}
	// Body
	auto body_v = body->compile(c);
	/*
	for (int i = 0; i < 15; ++i) {
		body->compile(c);
	}
	*/

	if (output_v && body_v.v) {
		VM::push_move_array(c.F, type.getElementType(), output_v, body_v.v);
	}
	// it++
	jit_insn_label(c.F, &label_it);
	c.iterator_increment(it);

	// jump to cond
	jit_insn_branch(c.F, &label_cond);

	c.leave_loop();

	// end label:
	jit_insn_label(c.F, &label_end);

	jit_value_t return_v = VM::clone_obj(c.F, output_v); // otherwise it is delete by the c.leave_block
	c.leave_block(c.F); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block
	return {return_v, type};
}

}
