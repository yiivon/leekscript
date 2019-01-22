#include "Block.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Throw.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

Block::Block() {
	type = {};
}

Block::~Block() {
	for (Instruction* instruction : instructions) {
		delete instruction;
	}
}

void Block::print(ostream& os, int indent, bool debug, bool condensed) const {
	if (!condensed) {
		os << "{";
		os << endl;
	}
	for (auto& instruction : instructions) {
		if (!condensed) os << tabs(indent + 1);
		instruction->print(os, indent + 1, debug);
		if (!condensed) os << endl;
	}
	if (!condensed) os << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
		if (may_return) os << " ==>" << return_type;
	}
}

Location Block::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void Block::analyse_global_functions(SemanticAnalyser* analyser) {
	int global_count = 0;
	for (size_t i = 0; i < instructions.size(); ++i) {
		if (auto vd = dynamic_cast<VariableDeclaration*>(instructions.at(i))) {
			if (vd->global && vd->function) {
				instructions.erase(instructions.begin() + i);
				instructions.insert(instructions.begin() + global_count, vd);
				global_count++;
				vd->analyse_global_functions(analyser);
			}
		}
	}
}

void Block::analyse(SemanticAnalyser* analyser) {

	// std::cout << "Block analyse " << req_type << std::endl;
	analyser->enter_block();

	type = {};

	for (unsigned i = 0; i < instructions.size(); ++i) {
		const auto& instruction = instructions.at(i);
		instruction->analyse(analyser);
		// Last instruction or instruction with a return somewhere
		if (i == instructions.size() - 1 or instruction->may_return) {
			type += instruction->type;
			was_reference = type.reference;
			// for (auto& t : instructions[i]->types) t.reference = false;
			type.reference = false;
			if (instruction->may_return) may_return = true;
			if (instruction->returning) returning = true;
			return_type += instruction->return_type;
		}
		// A return instruction
		if (instruction->returning) {
			analyser->leave_block();
			return; // no need to compile after a return
		}
	}
	// std::cout << "bloc type " << type << std::endl;

	analyser->leave_block();

	if (type == Type::mpz()) {
		type = Type::tmp_mpz();
	} else if (type == Type::tmp_mpz()) {
		temporary_mpz = true;
	}
}

Compiler::value Block::compile(Compiler& c) const {

	// std::cout << "Compile block " << type << std::endl;

	c.enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {

		std::ostringstream oss;
		instructions[i]->print(oss, 0, false);
		auto str = oss.str();
		auto pos = str.find('\n');
		if (pos != std::string::npos) str = str.substr(0, pos);
		c.log_insn_code(str);

		auto val = instructions[i]->compile(c);

		if (instructions[i]->returning) {
			// no need to compile after a return
			c.leave_block(false); // Variables already deleted by the return instruction
			return {};
		}
		if (i < instructions.size() - 1) {
			if (val.v != nullptr && !instructions[i]->type.is_void()) {
				c.insn_delete_temporary(val);
			}
		} else {
			if (type.must_manage_memory() and val.v != nullptr) {
				auto ret = c.insn_call(type, {val}, +[](LSValue* value) {
					return value->move();
				});
				c.leave_block();
				return ret;
			} else if (type == Type::tmp_mpz() && !temporary_mpz) {
				auto v = c.insn_clone_mpz(val);
				// c.insn_call({}, {v}, +[](__mpz_struct v) {
				// 	std::cout << "ret alloc = " << v._mp_alloc << std::endl;
				// 	std::cout << "ret size = " << v._mp_size << std::endl;
				// 	std::cout << "ret d = " << v._mp_d << std::endl;
				// 	std::cout << "mpz = ";
				// 	Compiler::print_mpz(v);
				// 	std::cout << std::endl;
				// });
				c.leave_block();
				return v;
			} else {
				c.leave_block();
				return val;
			}
		}
	}
	c.leave_block();
	return {nullptr, {}};
}

Value* Block::clone() const {
	auto b = new Block();
	for (const auto& i : instructions) {
		b->instructions.push_back(i->clone());
	}
	return b;
}

}
