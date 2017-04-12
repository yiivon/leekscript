#include "AbsoluteValue.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
	type = Type::INTEGER;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os, int, bool debug) const {
	os << "|";
	expression->print(os, debug);
	os << "|";
	if (debug) {
		os << " " << type;
	}
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	expression->analyse(analyser, Type::POINTER);
	constant = expression->constant;
	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

Compiler::value AbsoluteValue::compile(Compiler& c) const {
	auto ex = expression->compile(c);
	auto abso = c.insn_call(Type::INTEGER, {ex}, (void*) +[](LSValue* v) {
		return v->abso();
	});
	c.insn_delete_temporary(ex);
	if (type.nature == Nature::POINTER) {
		abso = c.insn_to_pointer(abso);
	}
	return abso;
}

}
