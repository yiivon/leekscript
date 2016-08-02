#include "../../compiler/instruction/ExpressionInstruction.hpp"

using namespace std;

namespace ls {

ExpressionInstruction::ExpressionInstruction(Value* value) {
	this->value = value;
}

ExpressionInstruction::~ExpressionInstruction() {
	delete this->value;
}

void ExpressionInstruction::print(ostream& os, bool debug) const {
	value->print(os, debug);
}

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	value->analyse(analyser, req_type);
	type = value->type;
	can_return = value->can_return;
}

jit_value_t ExpressionInstruction::compile(Compiler& c) const {
	return value->compile(c);
}

}
