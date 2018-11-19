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

	// Create blocks for the then and else cases. Insert the 'then' block at the end of the function.
	auto label_then = c.insn_init_label("then");
	auto label_else = c.insn_init_label("else");
	auto label_end = c.insn_init_label("end");

	Compiler::value then_v;
	Compiler::value else_v;

	// Compiler::value res;
	// if (type != Type::VOID) {
	// 	res = c.insn_create_value(type);
	// }

	auto cond = condition->compile(c);
	condition->compile_end(c);

	if (condition->type.nature == Nature::POINTER) {
		auto cond_bool = c.insn_to_bool(cond);
		c.insn_delete_temporary(cond);
		c.insn_if_new(cond_bool, &label_then, &label_else);
	} else {
		c.insn_if_new(cond, &label_then, &label_else);
	}

	c.insn_label(&label_then);

	then_v = then->compile(c);
	then->compile_end(c);
	// if (then_v.v) {
		// c.insn_store(res, then_v);
	// }

	c.insn_branch(&label_end);
	// Codegen of 'Then' can change the current block, update ThenBB for the PHI.
	label_then.block = LLVMCompiler::Builder.GetInsertBlock();

	// if (elze != nullptr or type != Type::VOID) {
	// 	c.insn_branch(&label_end);
	// }

	// Emit else block.
	c.insn_label(&label_else);

	if (elze != nullptr) {
		else_v = elze->compile(c);
		elze->compile_end(c);
		// if (else_v.v) {
			// c.insn_store(res, else_v);
		// }
	} else if (type != Type::VOID) {
		else_v = c.new_null();
		// c.insn_store(res, c.new_null());
	}
	if (elze != nullptr or type != Type::VOID) {
		// c.insn_label(&label_end);
	}

	c.insn_branch(&label_end);
	// Codegen of 'Else' can change the current block, update ElseBB for the PHI.
	label_else.block = LLVMCompiler::Builder.GetInsertBlock();

	// Emit merge block.
	c.insn_label(&label_end);
	auto PN = LLVMCompiler::Builder.CreatePHI(type.llvm_type(), 2, "iftmp");
	if (then_v.v) {
		PN->addIncoming(then_v.v, label_then.block);
	}
	if (else_v.v) {
		PN->addIncoming(else_v.v, label_else.block);
	}
	return {PN, type};
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
