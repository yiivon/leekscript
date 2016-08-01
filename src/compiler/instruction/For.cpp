#include "For.hpp"

#include "../value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Block.hpp"

using namespace std;

namespace ls {

For::For() {
	condition = nullptr;
	body = nullptr;
}

For::~For() {
	for (auto v : variablesValues) {
		delete v;
	}
	if (condition != nullptr) {
		delete condition;
	}
	for (auto i : iterations) {
		delete i;
	}
	delete body;
}

void For::print(ostream& os) const {
	os << "for ";
	if (variables.size() > 0) {
		os << "let ";
	}
	for (unsigned i = 0; i < variables.size(); ++i) {
		os << variables.at(i);
		if ((Value*) variablesValues.at(i) != nullptr) {
			os << " = ";
			variablesValues.at(i)->print(os);
		}
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
	os << "; ";
	if (condition != nullptr) {
		condition->print(os);
	}
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

		Token* var = variables[i];

		//cout << "var " << var->content << " : " << std::boolalpha <<  declare_variables[i] << endl;

		Type info = Type::POINTER;
		Value* value = nullptr;
		if (i < variablesValues.size()) {
			info = variablesValues[i]->type;
			value = variablesValues[i];
		}
		if (declare_variables[i]) {
			SemanticVar* v = analyser->add_var(var, info, value, nullptr);
			vars.insert(pair<string, SemanticVar*>(var->content, v));
		} else {
			SemanticVar* v = analyser->get_var(var);
			vars.insert(pair<string, SemanticVar*>(var->content, v));
		}
	}
	if (condition != nullptr) {
		condition->analyse(analyser);
	}
	for (auto it : iterations) {
		it->analyse(analyser);
	}
	analyser->enter_loop();
	body->analyse(analyser, req_type);
	analyser->leave_loop();
}

int for_is_true(LSValue* v) {
	return v->isTrue();
}

jit_value_t For::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	if (body->instructions.size() == 0 && condition == nullptr) {
		return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
	}

	// Initialization
	for (unsigned i = 0; i < variables.size(); ++i) {

		std::string& name = variables.at(i)->content;
		SemanticVar* v = vars.at(name);

		jit_value_t var;
		if (declare_variables[i]) {
			var = jit_value_create(F, JIT_INTEGER);
			c.add_var(name, var, v->type, false);
		} else {
			var = c.get_var(name).value;
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
	body->compile_jit(c, F, Type::VOID);

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

	if (req_type != Type::VOID) {
		return VM::create_null(F);
	}
	return jit_value_create_nint_constant(F, jit_type_int, 0);
}

}
