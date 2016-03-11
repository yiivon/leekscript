#include "Body.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSNumber.hpp"
#include "instruction/Return.hpp"
#include "../vm/VM.hpp"

using namespace std;

Body::Body() {
	type = Type::VALUE;
}

Body::~Body() {
	for (Instruction* instruction : instructions) {
		delete instruction;
	}
}

void Body::print(ostream& os) {
	os << "Body {" << endl;
	for (Instruction* instruction : instructions) {
		instruction->print(os);
		os << endl;
	}
	os << "}";
}

void Body::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	for (unsigned i = 0; i < instructions.size(); ++i) {
		instructions[i]->analyse(analyser, req_type);
		if (i == instructions.size() - 1) {
			type = instructions[i]->type;
		}
	}
}

jit_value_t Body::compile_jit(Compiler& c, jit_function_t& F, Type type) const {

	for (unsigned i = 0; i < instructions.size(); ++i) {

		if (i == instructions.size() - 1) {
			return instructions[i]->compile_jit(c, F, type);
		} else {
			instructions[i]->compile_jit(c, F, type);
		}
	}
	return JIT_CREATE_CONST_POINTER(F,LSNull::null_var);
}
