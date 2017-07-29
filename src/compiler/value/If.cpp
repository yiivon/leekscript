#include "If.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

If::If(bool ternary) {
	elze = nullptr;
	condition = nullptr;
	then = nullptr;
	type = Type::UNKNOWN;
	this->ternary = ternary;
}

If::~If() {
	delete condition;
	delete then;
	if (elze != nullptr) {
		delete elze;
	}
}

void If::print(ostream& os, int indent, bool debug) const {
	if (ternary) {
		os << "(";
		condition->print(os, indent, debug);
		os << " ? ";
		then->instructions[0]->print(os, indent, debug);
		os << " : ";
		elze->instructions[0]->print(os, indent, debug);
		os << ")";
	} else {
		os << "if ";
		condition->print(os, indent + 1, debug);
		os << " ";
		then->print(os, indent, debug);
		if (elze != nullptr) {
			os << " else ";
			elze->print(os, indent, debug);
		}
	}
	if (debug) {
		os << " " << types;
	}
}

Location If::location() const {
	return condition->location(); // TODO better
}

void If::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	types.clear();

	condition->analyse(analyser, Type::UNKNOWN);
	then->analyse(analyser, req_type);

	types.add(then->types);

	if (elze != nullptr) {

		elze->analyse(analyser, req_type);

		if (then->type == Type::VOID) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::VOID) { // elze contains return instruction
			type = then->type;
		} else {
			type = Type::get_compatible_type(then->type, elze->type);
		}
		types.add(elze->types);

		//then->analyse(analyser, type);

		//if (elze->type != type) {
		//	elze->analyse(analyser, type);
		//}
	} else {
		if (req_type == Type::VOID) {
			type = Type::VOID;
		} else {
			type = Type::POINTER; // Pointer because the else will give null
		}
		then->analyse(analyser, type);
		types = type;
	}
	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
}

Compiler::value If::compile(Compiler& c) const {

	Compiler::value res;
	if (type != Type::VOID) {
		res = c.insn_create_value(type);
	}

	Compiler::label label_else;
	Compiler::label label_end;

	auto cond = condition->compile(c);
	condition->compile_end(c);

	if (condition->type.nature == Nature::POINTER) {
		auto cond_bool = c.insn_to_bool(cond);
		c.insn_delete_temporary(cond);
		c.insn_branch_if_not(cond_bool, &label_else);
	} else {
		c.insn_branch_if_not(cond, &label_else);
	}

	auto then_v = then->compile(c);
	then->compile_end(c);
	if (then_v.v) {
		c.insn_store(res, then_v);
	}
	c.insn_branch(&label_end);

	c.insn_label(&label_else);

	if (elze != nullptr) {
		auto else_v = elze->compile(c);
		elze->compile_end(c);
		if (else_v.v) {
			c.insn_store(res, else_v);
		}
	} else {
		if (type != Type::VOID) {
			c.insn_store(res, c.new_null());
		}
	}
	c.insn_label(&label_end);
	return res;
}

Value* If::clone() const {
	auto iff = new If();
	iff->condition = condition->clone();
	iff->then = (Block*) then->clone();
	iff->elze = elze ? (Block*) elze->clone() : nullptr;
	iff->ternary = ternary;
	return iff;
}

}
