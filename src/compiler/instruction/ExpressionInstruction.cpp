#include "../../compiler/instruction/ExpressionInstruction.hpp"

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

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	if (req_type == Type::VOID) {
		value->analyse(analyser, Type::VOID);
		type = Type::VOID;
	} else {
		value->analyse(analyser, req_type);
		type = value->type;
		types = value->types;
	}
}

Compiler::value ExpressionInstruction::compile(Compiler& c) const {
	auto v = value->compile(c);
	if (type.nature == Nature::VOID) {
		c.insn_delete_temporary(v);
		return {nullptr, Type::UNKNOWN};
	} else {
		return v;
	}
}

}
