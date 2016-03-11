#include "../../vm/VM.hpp"
#include "For.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Number.hpp"

using namespace std;

For::For() {
	condition = nullptr;
	body = nullptr;
}

For::~For() {}

void For::print(ostream& os) const {
	os << "for ";
	if (variables.size() > 0) {
		os << "let ";
	}
	for (unsigned i = 0; i < variables.size(); ++i) {
		os << variables.at(i);
		if ((Value*)variablesValues.at(i) != nullptr) {
			os << " = ";
			variablesValues.at(i)->print(os);
		}
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
	os << "; ";
	if (condition != nullptr)
	condition->print(os);
	os << "; ";
	for (unsigned i = 0; i < iterations.size(); ++i) {
		iterations.at(i)->print(os);
		if (i < iterations.size() - 1) {
			os << ", ";
		}
	}
	os << " do" << endl;
	body->print(os);
	os << "end";
}

void For::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	for (unsigned i = 0; i < variablesValues.size(); ++i) {
		variablesValues[i]->analyse(analyser);
	}
	for (unsigned i = 0; i < variables.size(); ++i) {
		string var = variables[i];
		Type info = Type::POINTER;
		Value* value = nullptr;
		if (i < variablesValues.size()) {
			info = variablesValues[i]->type;
			value = variablesValues[i];
		}
		SemanticVar* v = analyser->add_var(var, info, value);
		vars.insert(pair<string, SemanticVar*>(var, v));
	}
	if (condition != nullptr) {
		condition->analyse(analyser);
	}
	for (auto it : iterations) {
		it->analyse(analyser);
	}
	body->analyse(analyser, req_type);
}

extern map<string, jit_value_t> globals;
extern map<string, jit_value_t> locals;

int for_is_true(LSValue* v) {
	return v->isTrue();
}

jit_value_t For::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	if (body->instructions.size() == 0 && condition == nullptr) {
		return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
	}

	// Initialization
	for (unsigned i = 0; i < variables.size(); ++i) {

		SemanticVar* v = vars.at(variables.at(i));

		jit_value_t var;

		if (v->scope == VarScope::GLOBAL) {
			auto previous_var = globals.find(variables[i]);
			if (previous_var == globals.end()) {
				var = jit_value_create(F, JIT_INTEGER);
				globals.insert(pair<string, jit_value_t>(variables[i], var));
			} else
				var = previous_var->second;
		} else {
			auto previous_var = locals.find(variables[i]);
			if (previous_var == locals.end()) {
				var = jit_value_create(F, JIT_INTEGER);
				locals.insert(pair<string, jit_value_t>(variables[i], var));
			} else
				var = previous_var->second;
		}
		if (variablesValues.at(i) != nullptr) {
			jit_value_t val = variablesValues.at(i)->compile_jit(c, F, Type::NEUTRAL);
			jit_insn_store(F, var, val);
		} else {
			jit_value_t val = JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
			jit_insn_store(F, var, val);
		}
	}

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_it = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;
	jit_value_t const_true = JIT_CREATE_CONST(F, JIT_INTEGER, 1);
	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 1, 0);

	c.enter_loop(&label_end, &label_it);

	// condition label:
	jit_insn_label(F, &label_cond);

	// condition
	jit_value_t cond = condition->compile_jit(c, F, Type::NEUTRAL);

	// goto end if !condition
	if (condition->type.nature == Nature::VALUE) {
		jit_insn_branch_if_not(F, cond, &label_end);
	} else {
		jit_value_t cond_bool = jit_insn_call_native(F, "is_true", (void*) for_is_true, sig, &cond, 1, JIT_CALL_NOTHROW);
		jit_value_t cmp = jit_insn_ne(F, cond_bool, const_true);
		jit_insn_branch_if(F, cmp, &label_end);
	}

	// body
	body->compile_jit(c, F, Type::NEUTRAL);

	jit_insn_label(F, &label_it);

	// iterations
	for (Value* it : iterations) {
		it->compile_jit(c, F, Type::NEUTRAL);
	}

	// jump to condition
	jit_insn_branch(F, &label_cond);

	// end label:
	jit_insn_label(F, &label_end);

	c.leave_loop();

	return JIT_CREATE_CONST_POINTER(F,LSNull::null_var);
}
