#include "Block.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../instruction/Return.hpp"
#include "../../vm/VM.hpp"

using namespace std;

namespace ls {

Block::Block() {
	type = Type::NULLL;
}

Block::~Block() {
	for (Instruction* instruction : instructions) {
		delete instruction;
	}
}

void Block::print(ostream& os, int indent, bool debug) const {
	os << "{" << endl;
	for (Instruction* instruction : instructions) {
		instruction->print(os, indent + 1, debug);
		os << endl;
	}
	os << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
	}
}

unsigned Block::line() const {
	return 0;
}

void Block::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	analyser->enter_block();

	type = Type::UNKNOWN;

	for (unsigned i = 0; i < instructions.size(); ++i) {

		instructions[i]->analyse(analyser, req_type);

		can_return = can_return or instructions[i]->can_return;

		if (instructions[i]->can_return or (i == instructions.size() - 1)) {
			type = Type::get_compatible_type(type, instructions[i]->type);
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
//				VM::inc_refs_if_not_temp(c.F, val);
				jit_value_t ret = VM::move_obj(c.F, val);
				c.leave_block(c.F);
				return ret;
			} else {
				c.leave_block(c.F);
				return val;
			}
		} else {
			instructions[i]->compile(c);
		}
	}
	c.leave_block(c.F);

	if (type.nature != Nature::VOID) {
		return VM::create_null(c.F);
	}
	return nullptr;
}

}
