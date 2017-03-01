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
	}
}

Compiler::value ExpressionInstruction::compile(Compiler& c) const {

	auto v = value->compile(c);

	if (type.nature == Nature::VOID) {
		if (value->type.must_manage_memory()) {
			VM::delete_temporary(c.F, v.v);
		}
		if (value->type == Type::MPZ_TMP) {
			c.insn_delete_mpz(v);
		}
		return {nullptr, Type::UNKNOWN};
	} else {
		return v;
	}
}

}
