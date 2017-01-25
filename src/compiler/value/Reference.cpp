#include "../../compiler/value/Reference.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Reference::Reference(Token *variable)
	: Value(), variable(variable), var(nullptr)
{
	type = Type::VALUE;
}

Reference::~Reference() {}

void Reference::print(ostream& os, int, bool debug) const {
	os << "@" << variable->content;
	if (debug) {
		os << " " << type;
	}
}

unsigned Reference::line() const {
	return 0;
}

void Reference::will_be_in_array(SemanticAnalyser*) {
	in_array = true;
}

void Reference::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	var = analyser->get_var(variable);
	if (var != nullptr) {
		type = var->type;
		scope = var->scope;
		attr_types = var->attr_types;
		if (scope != VarScope::INTERNAL and var->function != analyser->current_function()) {
			capture_index = analyser->current_function()->capture(var);
			scope = VarScope::CAPTURE;
  		}
	}
	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
//	cout << "ref " << variable->content << " : " << type << endl;
}

extern map<string, jit_value_t> internals;

Compiler::value Reference::compile(Compiler& c) const {

	if (scope == VarScope::CAPTURE) {
		return c.insn_get_capture(capture_index, type);
	}

	jit_value_t v;

	if (scope == VarScope::INTERNAL) {

		v = internals[variable->content];

	} else if (scope == VarScope::LOCAL) {

		v = c.get_var(variable->content).value;

	} else { /* if (scope == VarScope::PARAMETER) */

		v = jit_value_get_param(c.F, 1 + var->index); // 1 offset for function ptr
	}

	if (var->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
		return {VM::value_to_pointer(c.F, v, var->type), type};
	}
	if (var->type.raw_type == RawType::INTEGER and type.raw_type == RawType::REAL) {
		return {VM::int_to_real(c.F, v), type};
	}
	return {v, type};
}

}
