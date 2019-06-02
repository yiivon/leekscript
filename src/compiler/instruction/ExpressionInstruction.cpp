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

void ExpressionInstruction::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {
	// std::cout << "ExpressionInstruction::analyze() " << is_void << std::endl;
	value->is_void = is_void;
	value->analyze(analyzer);
	if (req_type->is_void()) {
		type = Type::void_;
	} else {
		type = value->type;
	}
	returning = value->returning;
	may_return = value->may_return;
	return_type = value->return_type;
	throws = value->throws;
}

Compiler::value ExpressionInstruction::compile(Compiler& c) const {
	auto v = value->compile(c);
	value->compile_end(c);
	assert(!v.v or v.t->llvm_type(c) == v.v->getType());
	return v;
}

Instruction* ExpressionInstruction::clone() const {
	return new ExpressionInstruction(value->clone());
}

}
