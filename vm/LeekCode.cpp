#include "LeekCode.hpp"
#include <string>

LeekCode::LeekCode() {}

LeekCode::~LeekCode() {}

void LeekCode::var(string name, int index) {
	global_variables.insert(pair<string, int>(name, index));
}

int LeekCode::constant(LSValue* value) {
	constants.push_back(value);
	return constants.size() - 1;
}

int LeekCode::add(LC_InstructionType type) {
	if (type != NOP) {
		instructions.push_back(LC_Instruction(type, LC_InstructionValue(), LC_InstructionValue()));
	}
	return instructions.size() - 1;
}
int LeekCode::add(LC_InstructionType type, int x) {
	instructions.push_back(LC_Instruction(type, x, LC_InstructionValue()));
	return instructions.size() - 1;
}
int LeekCode::add(LC_InstructionType type, Register x) {
	instructions.push_back(LC_Instruction(type, x, LC_InstructionValue()));
	return instructions.size() - 1;
}
int LeekCode::add(LC_InstructionType type, int x, int y) {
	instructions.push_back(LC_Instruction(type, x, y));
	return instructions.size() - 1;
}
int LeekCode::add(LC_InstructionType type, Register x, int y) {
	instructions.push_back(LC_Instruction(type, x, y));
	return instructions.size() - 1;
}
int LeekCode::add(LC_InstructionType type, int x, Register y) {
	instructions.push_back(LC_Instruction(type, x, y));
	return instructions.size() - 1;
}
int LeekCode::add(LC_InstructionType type, Register x, Register y) {
	instructions.push_back(LC_Instruction(type, x, y));
	return instructions.size() - 1;
}

int LeekCode::pos() {
	return instructions.size();
}

ostream& operator << (ostream& os, LC_InstructionValue& v) {
	if (v.empty) {
		os << "_";
	} else if (v.integer) {
		os << v.value;
	} else {
		os << (char)(v.reg - 1 + 97) << 'x';
	}
	return os;
}

string LeekCode::instruction_name(LC_InstructionType type) {
	switch (type) {
		case NOP : return "nop";
		case DECL: return "decl";
		case PUSH_VAR: return "push_var";
		case LOAD_VAR: return "load_var";
		case LOAD_VAR_P: return "load_var_p";
		case STORE: return "store";
		case ADD: return "add";
		case SUB: return "sub";
		case MUL: return "mul";
		case DIV: return "div";
		case POW: return "pow";
		case ABS: return "abs";
		case PUSH_NUMBER: return "push_n";
		case LOAD_NUMBER: return "load_n";
		case PUSH_BOOL: return "push_b";
		case LOAD_BOOL: return "load_b";
		case PUSH_STRING: return "push_s";
		case LOAD_STRING: return "load_s";
		case PUSH_ARRAY_STACK: return "push_a_stack";
		case PUSH_ARRAY_AX: return "push_a_ax";
		case PUSH_ARRAY_BX: return "push_a_bx";
		case AND: return "and";
		case OR: return "or";
		case INC: return "inc";
		case DEC: return "dec";
		case PRE_INC: return "p_inc";
		case PRE_DEC: return "p_dec";
		case NOT: return "not";
		case EQUAL: return "equal";
		case MOD: return "mod";
		case ARRAY_ACCESS: return "array_access";
		case ARRAY_ACCESS_P: return "array_access_p";
		case PUSH_OBJECT: return "push_o";
		case OBJECT_ACCESS: return "obj_access";
		case OBJECT_ACCESS_P: return "obj_access_p";
		case JUMP: return "jump";
		case JUMP_IF: return "jump_if";
		case ADD_EQUAL: return "add_eq";
		case SUB_EQUAL: return "sub_eq";
		case MUL_EQUAL: return "mul_eq";
		case DIV_EQUAL: return "div_eq";
		case MOD_EQUAL: return "mod_eq";
		case POW_EQUAL: return "pow_eq";
		case PUSH_NULL: return "push_null";
		case LOW: return "low";
		case MINUS: return "minus";
		case SWAP: return "swap";
		case PUSH_FUNC: return "push_f";
		case LOAD_FUNC: return "load_f";
		case CALL_FUNC: return "call_f";
		case RET: return "ret";
		case STOP: return "stop";
		case SQRT: return "sqrt";
		case PUSH_LOCAL: return "push_local";
		case LOAD_LOCAL: return "load_local";
		case LOAD_LOCAL_P: return "load_local_p";
		case DECL_LOCAL: return "decl_local";
		case PRINT: return "print";
		case PRE_TILDE: return "pre_tilde";
		case TILDE: return "tilde";
		case TILDE_TILDE: return "tilde_tilde";
		case TILDE_TILDE_END: return "tilde_tilde_end";
		case JUMP_IF_NOT: return "jump_if_not";
		case JUMP_IF_LOW: return "jump_if_low";
		case JUMP_IF_NEQ: return "jump_if_neq";
		case COMPARE: return "compare";
		case MOV: return "mov";
		default: return "???";
	}
}
