#include "ExpressionInstruction.hpp"

using namespace std;

ExpressionInstruction::ExpressionInstruction(Value* value) {
	this->value = value;
}

ExpressionInstruction::~ExpressionInstruction() {}

void ExpressionInstruction::print(ostream& os) const {
	cout << "ExpInst {";
	value->print(os);
	cout << "}";
}

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	value->analyse(analyser, req_type);
	type = value->type;
}

jit_value_t ExpressionInstruction::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {
	return value->compile_jit(c, F, req_type);
}
