/*
 * Represent the LeekScript bytecode
 * Executable by the VM class
 */

#ifndef LEEKCODE_HPP_
#define LEEKCODE_HPP_

#include <string>
#include <vector>
#include <map>
#include "LSValue.hpp"
#include <jit/jit.h>

using namespace std;

enum LC_InstructionType {
	NOP,
	DECL,
	PUSH_VAR, LOAD_VAR,
	LOAD_VAR_P,
	STORE,
	PUSH_NUMBER, LOAD_NUMBER,
	PUSH_STRING, LOAD_STRING,
	PUSH_BOOL, LOAD_BOOL,
	PUSH_ARRAY_STACK, PUSH_ARRAY_AX, PUSH_ARRAY_BX,
	ADD,
	SUB, MUL, DIV, POW, ABS, AND, OR, LOW, EQUAL, MOD, NOT,
	INC, DEC, PRE_INC, PRE_DEC, MINUS, ARRAY_ACCESS, ARRAY_ACCESS_P,
	PUSH_OBJECT, OBJECT_ACCESS, OBJECT_ACCESS_P, JUMP, JUMP_IF,
	ADD_EQUAL, SUB_EQUAL, MUL_EQUAL, DIV_EQUAL, MOD_EQUAL, POW_EQUAL,
	PUSH_NULL, SWAP,

	PUSH_FUNC, LOAD_FUNC,
	CALL_FUNC, RET, STOP, SQRT,

	PUSH_LOCAL, LOAD_LOCAL, LOAD_LOCAL_P, DECL_LOCAL,

	PRINT,
	PRE_TILDE, TILDE, TILDE_TILDE, TILDE_TILDE_END, JUMP_IF_NOT,
	JUMP_IF_LOW, JUMP_IF_NEQ, COMPARE,
	MOV
};

enum Register {
	STACK, AX, BX, CX, DX
};

class LC_InstructionValue {
public:
	bool empty = true;
	bool integer = false;
	long int value = 0;
	Register reg;

	LC_InstructionValue() : reg() {};
	LC_InstructionValue(int x) : empty(false), integer(true), value(x), reg() {};
	LC_InstructionValue(Register r) : empty(false), integer(false), value(0), reg(r) {};

	friend ostream& operator << (ostream& os, LC_InstructionValue& v);
};


class LC_Instruction {
public:
	LC_InstructionType type;
	LC_InstructionValue x;
	LC_InstructionValue y;

	LC_Instruction(LC_InstructionType type, LC_InstructionValue x, LC_InstructionValue y) : type(type), x(x), y(y) {}
};

class LC_ByteInstruction {
public:
	int type;
	void* x;
	void* y;
};

class LeekCode {
public:

	int start = 0;
	map<string, int> global_variables;
	vector<LC_Instruction> instructions;
	vector<LC_ByteInstruction> byte_instructions;
	vector<LSValue*> constants;
	int current_register = 2; // Start at bx

	LeekCode();
	virtual ~LeekCode();

	void var(string name, int index);
	int constant(LSValue* value);

	int add(LC_InstructionType type);
	int add(LC_InstructionType type, int x);
	int add(LC_InstructionType type, Register x);
	int add(LC_InstructionType type, int x, int y);
	int add(LC_InstructionType type, int x, Register y);
	int add(LC_InstructionType type, Register x, int y);
	int add(LC_InstructionType type, Register x, Register y);

	int pos();

	static string instruction_name(LC_InstructionType type);
};

#endif
