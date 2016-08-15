#include "For.hpp"

#include "../instruction/Return.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

For::For() {
}

For::~For() {
}

void For::print(ostream& os, int indent, bool debug) const {
	os << "for";
	for (Instruction* ins : inits) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << "; ";
	condition->print(os, indent + 1, debug);
	os << ";";
	for (Instruction* ins : increments) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << " ";
	body->print(os, indent, debug);
}

void For::analyse(SemanticAnalyser* analyser, const Type&) {

	type = Type::VOID;

	analyser->enter_block();

	// Init
	for (Instruction* ins : inits) {
		ins->analyse(analyser, Type::VOID);
		if (dynamic_cast<Return*>(ins)) {
			analyser->leave_block();
			return;
		}
	}

	// Condition
	condition->analyse(analyser, Type::BOOLEAN);

	// Body
	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();

	// Increment
	analyser->enter_block();
	for (Instruction* ins : increments) {
		ins->analyse(analyser, Type::VOID);
		if (dynamic_cast<Return*>(ins)) {
			break;
		}
	}
	analyser->leave_block();

	analyser->leave_block();
}

bool for_is_true(LSValue* v) {
	return v->isTrue();
}

jit_value_t For::compile(Compiler& c) const {

	c.enter_block(); // { for init ; cond ; inc { body } }<-- this block

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_inc = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// Init
	for (Instruction* ins : inits) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			c.leave_block(c.F);
			return nullptr;
		}
	}

	// Cond
	jit_insn_label(c.F, &label_cond);
	jit_value_t cond = condition->compile(c);
	if (condition->type.nature == Nature::POINTER) {
		jit_type_t args_types[1] = {JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types, 1, 0);
		jit_value_t cond_bool = jit_insn_call_native(c.F, "is_true", (void*) for_is_true, sig, &cond, 1, JIT_CALL_NOTHROW);

		if (condition->type.must_manage_memory()) {
			VM::delete_temporary(c.F, cond);
		}

		jit_insn_branch_if_not(c.F, cond_bool, &label_end);
	} else {
		jit_insn_branch_if_not(c.F, cond, &label_end);
	}

	// Body
	c.enter_loop(&label_end, &label_inc);
	body->compile(c);
	c.leave_loop();
	jit_insn_label(c.F, &label_inc);

	// Inc
	c.enter_block();
	for (Instruction* ins : increments) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			break;
		}
	}
	c.leave_block(c.F);
	jit_insn_branch(c.F, &label_cond);

	// End
	jit_insn_label(c.F, &label_end);
	c.leave_block(c.F);
	return nullptr;
}

}
