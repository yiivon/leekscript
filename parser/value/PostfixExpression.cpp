#include "../../vm/VM.hpp"
#include "PostfixExpression.hpp"
#include "LeftValue.hpp"

using namespace std;

PostfixExpression::PostfixExpression() {
	expression = nullptr;
	operatorr = nullptr;
	return_value = true;
}

PostfixExpression::~PostfixExpression() {}

void PostfixExpression::print(ostream& os) const {
	expression->print(os);
	operatorr->print(os);
}

void PostfixExpression::analyse(SemanticAnalyser* analyser, const Type) {
	expression->analyse(analyser);
	type = expression->type;
	this->return_value = return_value;
}

extern LSValue* jit_inc(LSValue*);
extern LSValue* jit_dec(LSValue*);

jit_value_t PostfixExpression::compile_jit(Compiler& c, jit_function_t& F, Type	req_type) const {

	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
	vector<jit_value_t> args;

	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t ox = jit_insn_load(F, x);
				jit_value_t y = JIT_CREATE_CONST(F, JIT_INTEGER, 1);
				jit_value_t sum = jit_insn_add(F, x, y);
				jit_insn_store(F, x, sum);
				if (req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, ox, req_type);
				}
				return ox;
			} else {
				args.push_back(expression->compile_jit(c, F, Type::NEUTRAL));
				func = (void*) jit_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t ox = jit_insn_load(F, x);
				jit_value_t y = JIT_CREATE_CONST(F, JIT_INTEGER, 1);
				jit_value_t sum = jit_insn_sub(F, x, y);
				jit_insn_store(F, x, sum);
				if (req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, ox, req_type);
				}
				return ox;
			} else {
				args.push_back(expression->compile_jit(c, F, Type::NEUTRAL));
				func = (void*) jit_dec;
			}
			break;
		}
		default: {

		}
	}
	return jit_insn_call_native(F, "", func, sig, args.data(), 1, JIT_CALL_NOTHROW);
}
