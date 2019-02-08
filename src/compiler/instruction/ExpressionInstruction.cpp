#include "ExpressionInstruction.hpp"
#include "../value/Block.hpp"
#include "../value/If.hpp"

namespace ls {

ExpressionInstruction::ExpressionInstruction(Value* value) {
	this->value = value;
}

ExpressionInstruction::~ExpressionInstruction() {
	delete this->value;
}

void ExpressionInstruction::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	value->print(os, indent, debug, condensed);
}

Location ExpressionInstruction::location() const {
	return value->location();
}

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	value->analyse(analyser);
	if (req_type.is_void()) {
		type = {};
	} else {
		type = value->type;
	}
	returning = value->returning;
	may_return = value->may_return;
	return_type = value->return_type;
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
