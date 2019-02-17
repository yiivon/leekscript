#include "Foreach.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../semantic/SemanticAnalyser.hpp"

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

void Foreach::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "for ";

	if (key != nullptr) {
		os << key->content << " " << container->type.key();
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
		body->is_void = true;
	}
	analyser->enter_block();

	container->analyse(analyser);

	if (container->type._types.size() != 0 and not container->type.iterable() and not container->type.is_any()) {
		analyser->add_error({SemanticError::Type::VALUE_NOT_ITERABLE, container->location(), container->location(), {container->to_string(), container->type.to_string()}});
		return;
	}

	key_type = container->type.key();
	value_type = container->type.element();
	if (key != nullptr) {
		key_var = analyser->add_var(key.get(), key_type, nullptr, nullptr);
	}
	value_var = analyser->add_var(value.get(), value_type, nullptr, nullptr);

	analyser->enter_loop();
	body->analyse(analyser);
	if (req_type.is_array()) {
		type = Type::tmp_array(body->type);
	}
	analyser->leave_loop();
	analyser->leave_block();
}

Compiler::value Foreach::compile(Compiler& c) const {
	auto container_v = container->compile(c);
	return c.insn_foreach(container_v, type.element(), value->content, key ? key->content : "", [&](Compiler::value value, Compiler::value key) {
		return body->compile(c);
	});
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
