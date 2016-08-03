#include "Match.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

Match::Match() {}

Match::~Match() {
	delete value;
	for (auto& ps : pattern_list) {
		for (Pattern p : ps) {
			delete p.begin;
			delete p.end;
		}
	}
	for (auto x : returns) delete x;
}

void Match::print(std::ostream &os, int indent, bool debug) const {
	os << "match ";
	value->print(os, debug);
	for (size_t i = 0; i < pattern_list.size(); ++i) {

		os << endl << tabs(indent);

		const vector<Pattern>& list = pattern_list[i];
		for (size_t j = 0; j < list.size(); ++j) {
			if (j > 0) {
				os << "|";
			}
			list[j].print(os, indent, debug);
		}
		os << " : ";
		returns[i]->print(os, indent, debug);
	}
	os << " }";
	if (debug) {
		os << " " << type;
	}
}

unsigned Match::line() const
{
	return 0;
}

void Match::analyse(ls::SemanticAnalyser *analyser, const Type &req_type)
{

	bool any_pointer = false;

	value->analyse(analyser, Type::UNKNOWN);
	if (value->type.nature == Nature::POINTER) any_pointer = true;

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (any_pointer) break;

			if (p.begin) {
				p.begin->analyse(analyser, Type::UNKNOWN);
				if (p.begin->type.nature == Nature::POINTER) any_pointer = true;
			}
			if (p.end) {
				p.end->analyse(analyser, Type::UNKNOWN);
				if (p.end->type.nature == Nature::POINTER) any_pointer = true;
			}
		}
	}

	if (any_pointer) {
		value->analyse(analyser, Type::POINTER);
		for (auto& ps : pattern_list) {
			for (Pattern& p : ps) {
				if (p.begin) {
					p.begin->analyse(analyser, Type::POINTER);
				}
				if (p.end) {
					p.end->analyse(analyser, Type::POINTER);
				}
			}
		}
	}

	type = Type::UNKNOWN;

	for (size_t i = 0; i < returns.size(); ++i) {
		returns[i]->analyse(analyser);
		returns[i]->print(cout);
		cout << " type : " << returns[i]->type << endl;
		type = Type::get_compatible_type(type, returns[i]->type);
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

jit_value_t Match::compile(Compiler &c) const
{
	jit_value_t v = value->compile(c);

	jit_value_t res = jit_value_create(c.F, JIT_POINTER);
	jit_label_t label_next = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	for (size_t i = 0; i < pattern_list.size(); ++i) {
		if (i > 0) {
			jit_insn_label(c.F, &label_next);
			label_next = jit_label_undefined;
		}

		if (pattern_list[i].size() == 1) {
			jit_value_t cond = pattern_list[i][0].match(c, v);
			if (cond) {
				jit_insn_branch_if_not(c.F, cond, &label_next);
			}
		} else {
			jit_label_t label_match = jit_label_undefined;

			for (const Pattern& pattern : pattern_list[i]) {
				jit_value_t cond = pattern.match(c, v);
				if (cond) jit_insn_branch_if(c.F, cond, &label_match);
				else jit_insn_branch(c.F, &label_match); // mettre .. dans la liste est idiot donc il n'y a pas besoin que ce soit optimisé
			}

			jit_insn_branch(c.F, &label_next);
			jit_insn_label(c.F, &label_match);
		}

		jit_value_t ret = returns[i]->compile(c);
		jit_insn_store(c.F, res, ret);
		jit_insn_branch(c.F, &label_end);
	}
	jit_insn_label(c.F, &label_next);

	jit_insn_store(c.F, res, VM::create_null(c.F));

	jit_insn_label(c.F, &label_end);
	return res;
}

Match::Pattern::Pattern(Value *value)
	: interval(false), begin(value), end(nullptr)
{
}

Match::Pattern::Pattern(Value *begin, Value *end)
	: interval(true), begin(begin), end(end)
{
}

Match::Pattern::~Pattern()
{
}

void Match::Pattern::print(ostream &os, int indent, bool debug) const
{
	if (interval) {
		if (begin) begin->print(os, indent, debug);
		os << "..";
		if (end) end->print(os, indent, debug);
	} else {
		begin->print(os, indent, debug);
	}
}

bool jit_equals_(LSValue* x, LSValue* y) {
	return x->operator == (y);
}
bool jit_less_(LSValue* x, LSValue* y) {
	return y->operator < (x);
}
bool jit_greater_equal_(LSValue* x, LSValue* y) {
	return y->operator >= (x);
}

jit_value_t Match::Pattern::match(Compiler &c, jit_value_t v) const
{
	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types, 2, 0);

	if (interval) {
		jit_value_t ge = nullptr;
		if (begin) {
			jit_value_t b = begin->compile(c);
			if (begin->type.nature == Nature::VALUE) {
				ge = jit_insn_ge(c.F, v, b);
			} else {
				jit_value_t args[2] = { v, b };
				ge = jit_insn_call_native(c.F, "", (void*) jit_greater_equal_, sig, args, 2, JIT_CALL_NOTHROW);
			}
		}

		jit_value_t lt = nullptr;
		if (end) {
			jit_value_t e = end->compile(c);
			if (end->type.nature == Nature::VALUE) {
				lt = jit_insn_lt(c.F, v, e);
			} else {
				jit_value_t args[2] = { v, e };
				lt = jit_insn_call_native(c.F, "", (void*) jit_less_, sig, args, 2, JIT_CALL_NOTHROW);
			}
		}

		if (ge) {
			if (lt) {
				return jit_insn_and(c.F, ge, lt);
			} else {
				return ge;
			}
		} else {
			if (lt) {
				return lt;
			} else {
				return nullptr; // équivalent à default
			}
		}

	} else {
		jit_value_t cond;
		jit_value_t p = begin->compile(c);

		if (begin->type.nature == Nature::VALUE) {
			cond = jit_insn_eq(c.F, v, p);
		} else {
			jit_value_t args[2] = { v, p };
			cond = jit_insn_call_native(c.F, "", (void*) jit_equals_, sig, args, 2, JIT_CALL_NOTHROW);
		}
		return cond;
	}
}

}
