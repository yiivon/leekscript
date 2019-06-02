#include "If.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "Number.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Continue.hpp"
#include "../../vm/LSValue.hpp"

namespace ls {

If::If(bool ternary) {
	elze = nullptr;
	condition = nullptr;
	then = nullptr;
	type = Type::void_;
	this->ternary = ternary;
}

If::~If() {
	delete condition;
	delete then;
	if (elze != nullptr) {
		delete elze;
	}
}

void If::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (ternary) {
		os << "(";
		condition->print(os, indent, debug);
		os << " ? ";
		then->instructions[0]->print(os, indent, debug, condensed);
		os << " : ";
		elze->instructions[0]->print(os, indent, debug, condensed);
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
		os << " " << type;
	}
}

Location If::location() const {
	return condition->location(); // TODO better
}

void If::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "If " << is_void << std::endl;
	
	condition->analyze(analyzer);
	throws = condition->throws;

	then->is_void = is_void;
	then->analyze(analyzer);

	type = then->type;

	if (elze != nullptr) {
		elze->is_void = is_void;
		elze->analyze(analyzer);
		if (type->is_void() and not elze->type->is_void() and not then->returning) {
			type = Type::null;
		}
		type = type->operator + (elze->type);
	} else if (not type->is_void() or then->returning) {
		type = type->operator + (Type::null);
	}
	if (is_void) type = Type::void_;
	throws |= then->throws or (elze != nullptr and elze->throws);
	returning = then->returning and (elze != nullptr and elze->returning);
	may_return = then->may_return or (elze != nullptr and elze->may_return);
	return_type = return_type->operator + (then->return_type);
	if (elze != nullptr) return_type = return_type->operator + (elze->return_type);
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

	then_v = c.insn_convert(then->compile(c), type->fold());
	if (!then_v.v) then_v = c.insn_convert(c.new_null(), type->fold());
	then->compile_end(c);

	c.insn_branch(&label_end);
	label_then.block = c.builder.GetInsertBlock();

	c.insn_label(&label_else);

	if (elze != nullptr) {
		else_v = c.insn_convert(elze->compile(c), type->fold());
		elze->compile_end(c);
	} else if (not type->is_void()) {
		else_v = c.insn_convert(c.new_null(), type->fold());
	}

	c.insn_branch(&label_end);
	label_else.block = c.builder.GetInsertBlock();

	c.insn_label(&label_end);
	
	if (type->is_void()) {
		return {};
	} else {
		return c.insn_phi(type, then_v, label_then, else_v, label_else);
	}
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
