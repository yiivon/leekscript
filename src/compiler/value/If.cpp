#include "If.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "Number.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Continue.hpp"
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

void If::print(ostream& os, int indent, bool debug, bool condensed) const {
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

		then->analyse(analyser, type);

		if (elze->type != type) {
			elze->analyse(analyser, type);
		}
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

	auto label_then = c.insn_init_label("then");
	auto label_else = c.insn_init_label("else");
	auto label_end = c.insn_init_label("end");
	Compiler::value then_v;
	Compiler::value else_v;

	auto cond = condition->compile(c);
	condition->compile_end(c);
	auto cond_boolean = c.insn_to_bool(cond);
	c.insn_delete_temporary(cond);
	c.insn_if_new(cond_boolean, &label_then, &label_else);

	c.insn_label(&label_then);

	then_v = then->compile(c);
	then->compile_end(c);

	if (dynamic_cast<Return*>(then->instructions[0]) == nullptr && dynamic_cast<Break*>(then->instructions[0]) == nullptr && dynamic_cast<Continue*>(then->instructions[0]) == nullptr) {
		c.insn_branch(&label_end);
	}
	label_then.block = LLVMCompiler::Builder.GetInsertBlock();

	c.insn_label(&label_else);

	if (elze != nullptr) {
		else_v = elze->compile(c);
		elze->compile_end(c);
	} else if (type != Type::VOID) {
		else_v = c.new_null();
	}

	c.insn_branch(&label_end);
	label_else.block = LLVMCompiler::Builder.GetInsertBlock();

	c.insn_label(&label_end);
	if (type != Type::VOID) {
		auto phi = LLVMCompiler::Builder.CreatePHI(type.llvm_type(), 2, "iftmp");
		if (then_v.v) phi->addIncoming(then_v.v, label_then.block);
		if (else_v.v) phi->addIncoming(else_v.v, label_else.block);
		return {phi, type};
	}
	return {nullptr, Type::VOID};
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
