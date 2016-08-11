#include "../../compiler/value/PostfixExpression.hpp"

#include "../../compiler/value/LeftValue.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

PostfixExpression::PostfixExpression() {
	expression = nullptr;
	operatorr = nullptr;
	return_value = true;
}

PostfixExpression::~PostfixExpression() {
	delete expression;
	delete operatorr;
}

void PostfixExpression::print(ostream& os, int indent, bool debug) const {
	expression->print(os, indent, debug);
	operatorr->print(os);
	if (debug) {
		os << " " << type;
	}
}

unsigned PostfixExpression::line() const {
	return 0;
}

void PostfixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	expression->analyse(analyser, Type::UNKNOWN);
	type = expression->type;
	this->return_value = return_value;

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

extern LSValue* jit_inc(LSValue*);
extern LSValue* jit_dec(LSValue*);

jit_value_t PostfixExpression::compile(Compiler& c) const {

	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
	vector<jit_value_t> args;

	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t ox = jit_insn_load(c.F, x);
				jit_value_t y = JIT_CREATE_CONST(c.F, JIT_INTEGER, 1);
				jit_value_t sum = jit_insn_add(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, ox, type);
				}
				return ox;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t ox = jit_insn_load(c.F, x);
				jit_value_t y = JIT_CREATE_CONST(c.F, JIT_INTEGER, 1);
				jit_value_t sum = jit_insn_sub(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, ox, type);
				}
				return ox;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_dec;
			}
			break;
		}
		default: {}
	}
	return jit_insn_call_native(c.F, "", func, sig, args.data(), 1, JIT_CALL_NOTHROW);
}

}
