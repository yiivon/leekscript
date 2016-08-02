#include "Block.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../instruction/Return.hpp"
#include "../../vm/VM.hpp"

using namespace std;

namespace ls {

Block::Block() {
	type = Type::UNKNOWN;
}

Block::~Block() {
	for (Instruction* instruction : instructions) {
		delete instruction;
	}
}

void Block::print(ostream& os, bool debug) const {
	os << "{" << endl;
	for (Instruction* instruction : instructions) {
		os << "    ";
		instruction->print(os, debug);
		os << endl;
	}
	os << "}";
}

unsigned Block::line() const {
	return 0;
}

void Block::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	analyser->enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {

		if (i == instructions.size() - 1) {
			instructions[i]->analyse(analyser, req_type);
			type = instructions[i]->type;
		} else {
			instructions[i]->analyse(analyser, Type::VOID);
		}

		if (instructions[i]->can_return) {
			can_return = true;
		}
	}

	analyser->leave_block();

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

jit_value_t Block::compile(Compiler& c) const {

	c.enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {

		if (i == instructions.size() - 1) {
			jit_value_t val = instructions[i]->compile(c);
			if (type.must_manage_memory()) {
				VM::inc_refs(c.F, val);
			}
			c.leave_block(c.F);
			return val;
		} else {
			//jit_value_t res = instructions[i]->compile_jit(c, F, Type::POINTER);
			//VM::delete_temporary(F, res);
			instructions[i]->compile(c);
		}
	}
	c.leave_block(c.F);

	if (type.nature != Nature::VOID) {
		return VM::create_null(c.F);
	}
	return jit_value_create_nint_constant(c.F, jit_type_int, 0);
}

}
