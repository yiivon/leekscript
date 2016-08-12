#include "../../compiler/instruction/While.hpp"

#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

While::While() {
	condition = nullptr;
	body = nullptr;
}

While::~While() {
	delete condition;
	delete body;
}

void While::print(ostream& os, int indent, bool debug) const {
	os << tabs(indent) << "while ";
	condition->print(os, debug);
	os << " ";
	body->print(os, indent, debug);
}

void While::analyse(SemanticAnalyser* analyser, const Type&) {

	if (condition != nullptr) {
		condition->analyse(analyser, Type::UNKNOWN);
	}
	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();
}

int while_is_true(LSValue* v) {
	return v->isTrue();
}

jit_value_t While::compile(Compiler& c) const {

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;
	jit_value_t const_true = JIT_CREATE_CONST(c.F, JIT_INTEGER, 1);
	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 1, 0);

	c.enter_loop(&label_end, &label_cond);

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// condition
	jit_value_t cond = condition->compile(c);

	// goto end si !condition
	if (condition->type.nature == Nature::VALUE) {
		jit_insn_branch_if_not(c.F, cond, &label_end);
	} else {
		jit_value_t cond_bool = jit_insn_call_native(c.F, "is_true", (void*) while_is_true, sig, &cond, 1, JIT_CALL_NOTHROW);
		jit_value_t cmp = jit_insn_ne(c.F, cond_bool, const_true);
		jit_insn_branch_if(c.F, cmp, &label_end);
	}

	// body
	body->compile(c);

	// jump to cond
	jit_insn_branch(c.F, &label_cond);

	// end label:
	jit_insn_label(c.F, &label_end);

	c.leave_loop();

	return nullptr;
}

}
