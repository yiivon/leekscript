#include "../../vm/VM.hpp"
#include "While.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Number.hpp"

using namespace std;

While::While() {
	condition = nullptr;
	body = nullptr;
}

While::~While() {}

void While::print(ostream& os) const {
	os << "while ";
	condition->print(os);
	os << " do" << endl;
	body->print(os);
	os << "end";
}

void While::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (condition != nullptr) {
		condition->analyse(analyser);
	}
	body->analyse(analyser, req_type);
}

int while_is_true(LSValue* v) {
	return v->isTrue();
}

jit_value_t While::compile_jit(Compiler& c, jit_function_t& F, Type type) const {

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;
	jit_value_t const_true = JIT_CREATE_CONST(F, JIT_INTEGER, 1);
	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 1, 0);

	c.enter_loop(&label_end, &label_cond);

	// cond label:
	jit_insn_label(F, &label_cond);

	// condition
	jit_value_t cond = condition->compile_jit(c, F, Type::NEUTRAL);

	// goto end si !condition
	if (condition->type.nature == Nature::VALUE) {
		jit_insn_branch_if_not(F, cond, &label_end);
	} else {
		jit_value_t cond_bool = jit_insn_call_native(F, "is_true", (void*) while_is_true, sig, &cond, 1, JIT_CALL_NOTHROW);
		jit_value_t cmp = jit_insn_ne(F, cond_bool, const_true);
		jit_insn_branch_if(F, cmp, &label_end);
	}

	// body
	body->compile_jit(c, F, Type::NEUTRAL);

	// jump to cond
	jit_insn_branch(F, &label_cond);

	// end label:
	jit_insn_label(F, &label_end);

	c.leave_loop();

	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}
