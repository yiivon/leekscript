#include "../../compiler/value/If.hpp"

#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

If::If() {
	elze = nullptr;
	condition = nullptr;
	then = nullptr;
	inversed = false;
}

If::~If() {
	delete condition;
	delete then;
	if (elze != nullptr) {
		delete elze;
	}
}

void If::print(ostream& os) const {
	os << "if ";
	condition->print(os);
	os << " then" << endl;
	then->print(os);
	if (elze != nullptr) {
		os << "else" << endl;
		elze->print(os);
	}
	os << "end";
}

void If::analyse(SemanticAnalyser* analyser, const Type req_type) {

	type = Type::UNKNOWN;

	condition->analyse(analyser);
	then->analyse(analyser, req_type);

	if (then->type.nature == Nature::POINTER) type.nature = Nature::POINTER;

	if (elze != nullptr) {
		elze->analyse(analyser, req_type);
		if (elze->type.nature == Nature::POINTER) type.nature = Nature::POINTER;
	}

	if (Expression* cond_ex = dynamic_cast<Expression*>(condition)) {
		if (cond_ex->op->type == TokenType::DOUBLE_EQUAL) {
			if (Number* v2n = dynamic_cast<Number*>(cond_ex->v2)) {
				if (v2n->value == 0) {
					inversed = true;
					condition = cond_ex->v1;
				}
			}
		}
	}
}

int is_true(LSValue* v) {
	return v->isTrue();
}

jit_value_t If::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	jit_value_t res = jit_value_create(F, JIT_INTEGER);
	jit_label_t label_else = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	jit_value_t cond = condition->compile_jit(c, F, Type::BOOLEAN);

	if (condition->type.nature == Nature::POINTER) {

		jit_value_t const_true = jit_value_create_nint_constant(F, jit_type_int, 1);

		jit_type_t args_types[1] = {JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 1, 0);
		jit_value_t cond_bool = jit_insn_call_native(F, "is_true", (void*) is_true, sig, &cond, 1, JIT_CALL_NOTHROW);

		jit_value_t cmp = inversed ? jit_insn_eq(F, cond_bool, const_true) : jit_insn_ne(F, cond_bool, const_true);
		jit_insn_branch_if(F, cmp, &label_else);

	} else {

		inversed ? jit_insn_branch_if(F, cond, &label_else) : jit_insn_branch_if_not(F, cond, &label_else);
	}

	jit_value_t then_v = then->compile_jit(c, F, req_type);
	jit_insn_store(F, res, then_v);
	jit_insn_branch(F, &label_end);

	jit_insn_label(F, &label_else);

	if (elze != nullptr) {
		jit_value_t else_v = elze->compile_jit(c, F, req_type);
		jit_insn_store(F, res, else_v);
	} else {
		if (req_type.nature == Nature::POINTER) {
			LSValue* n = LSNull::null_var;
			jit_insn_store(F, res, JIT_CREATE_CONST_POINTER(F, n));
		} else {
			jit_insn_store(F, res, jit_value_create_nint_constant(F, JIT_INTEGER, 12));
		}
	}

	jit_insn_label(F, &label_end);

	return res;
}

}
