#include "Match.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

Match::Match()
{

}

Match::~Match()
{
	delete value;
	for (auto x : patterns) delete x;
	for (auto x : returns) delete x;
}

void Match::print(std::ostream &os, bool debug) const
{
	os << "match ";
	value->print(os, debug);
	os << " { ";
	for (size_t i = 0; i < patterns.size(); ++i) {
		patterns[i]->print(os, debug);
		os << " : ";
		returns[i]->print(os, debug);
		os << ", ";
	}
	if (returns.size() > patterns.size()) {
		os << "default : ";
		returns[patterns.size()]->print(os, debug);
	}
	os << " }";
}

unsigned Match::line() const
{
	return 0;
}

void Match::analyse(ls::SemanticAnalyser *analyser, const Type &req_type)
{
	type = req_type;

	value->analyse(analyser, Type::UNKNOWN);

	for (size_t i = 0; i < patterns.size(); ++i) {
		patterns[i]->analyse(analyser, value->type);
	}
	for (size_t i = 0; i < returns.size(); ++i) {
		returns[i]->analyse(analyser, req_type);
	}
}

bool jit_equals_(LSValue* x, LSValue* y) {
	return x->operator == (y);
}

jit_value_t Match::compile(Compiler &c) const
{
	jit_value_t v = value->compile(c);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types, 2, 0);

	jit_value_t res = jit_value_create(c.F, JIT_INTEGER);
	jit_label_t label_next = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	for (size_t i = 0; i < patterns.size(); ++i) {
		if (i > 0) {
			jit_insn_label(c.F, &label_next);
			label_next = jit_label_undefined;
		}

		jit_value_t cond;
		jit_value_t p = patterns[i]->compile(c);

		if (value->type.nature == Nature::VALUE && patterns[i]->type.nature == Nature::VALUE) {
			cond = jit_insn_eq(c.F, v, p);
		} else {
			jit_value_t args[2] = { v, p };
			cond = jit_insn_call_native(c.F, "", (void*) jit_equals_, sig, args, 2, JIT_CALL_NOTHROW);
		}

		jit_insn_branch_if_not(c.F, cond, &label_next);

		jit_value_t ret = returns[i]->compile(c);
		jit_insn_store(c.F, res, ret);
		jit_insn_branch(c.F, &label_end);
	}
	jit_insn_label(c.F, &label_next);

	if (returns.size() > patterns.size()) {
		jit_value_t ret = returns[patterns.size()]->compile(c);
		jit_insn_store(c.F, res, ret);
	}

	jit_insn_label(c.F, &label_end);
	return res;
}

}
