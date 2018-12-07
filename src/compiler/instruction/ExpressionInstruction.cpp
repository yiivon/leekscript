#include "ExpressionInstruction.hpp"

using namespace std;

namespace ls {

ExpressionInstruction::ExpressionInstruction(Value* value) {
	this->value = value;
}

ExpressionInstruction::~ExpressionInstruction() {
	delete this->value;
}

void ExpressionInstruction::print(ostream& os, int indent, bool debug) const {
	value->print(os, indent, debug);
}

Location ExpressionInstruction::location() const {
	return value->location();
}

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	if (req_type._types.size() == 0) {
		value->analyse(analyser);
		type = {};
	} else {
		value->analyse(analyser);
		type = value->type;
	}
}

Compiler::value ExpressionInstruction::compile(Compiler& c) const {
	auto v = value->compile(c);
	value->compile_end(c);
	assert(!v.v or v.t.llvm_type() == v.v->getType());
	return v;
}

Instruction* ExpressionInstruction::clone() const {
	return new ExpressionInstruction(value->clone());
}

}
