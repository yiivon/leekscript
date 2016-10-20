#include "OperatorPrototypes.hpp"

namespace ls {

bool OperatorPrototypes::Method::NATIVE = true;

OperatorPrototypes::Method::Method(std::initializer_list<Type> args, Type return_type, void* addr, bool native) {
	this->addr = addr;
	type = {RawType::FUNCTION, Nature::POINTER};
	type.setReturnType(return_type);
	for (Type arg : args) {
		type.addArgumentType(arg);
	}
	this->native = native;
}

void OperatorPrototypes::prototype(std::initializer_list<std::string> names, int precedence, bool left_assignment,
	bool right_assignment, std::initializer_list<Method>) {

	// TODO
}

/*
 * Binary operators precedence
 * ------------------
 * 0| ~ ~~ ~= ~~=
 * ------------------
 * 1| ^
 * ------------------
 * 2| * / % \
 * ------------------
 * 3| + -
 * ------------------
 * 4| < <= > >= instanceof
 * ------------------
 * 5| == != === !===
 * ------------------
 * 6| && and
 * ------------------
 * 7| || or xor
 * ------------------
 * 8| ??
 * ------------------
 * 9| = += -= *= /= %= ^= <=>
 */

/** Static method to initialize all the available operators */
void OperatorPrototypes::initialize() {

	prototype({"!"}, -1, false, false, {
		{{Type::POINTER}, Type::BOOLEAN, (void*) &not_pointer}
	});

	prototype({"="}, 9, true, false, {
		{{Type::FLOAT, Type::FLOAT}, Type::FLOAT, (void*) &store_real_real, Method::NATIVE},
		{{Type::INTEGER, Type::INTEGER}, Type::INTEGER, (void*) &store_real_real, Method::NATIVE},
	});

	prototype({"+"}, 3, false, false, {
		{{Type::FLOAT, Type::FLOAT}, Type::FLOAT, (void*) &add_real_real, Method::NATIVE},
		{{Type::INTEGER, Type::INTEGER}, Type::INTEGER, (void*) &add_real_real, Method::NATIVE},
	});

	prototype({"+="}, 9, true, false, {
		{{Type::FLOAT, Type::FLOAT}, Type::FLOAT, (void*) &add_equals_real_real, Method::NATIVE},
		{{Type::INTEGER, Type::INTEGER}, Type::INTEGER, (void*) &add_equals_real_real, Method::NATIVE},
	});
}

bool OperatorPrototypes::not_pointer(LSValue* value) {
	return value->ls_not();
}
jit_value_t OperatorPrototypes::not_value(Compiler& c, jit_value_t v) {
	return jit_insn_to_not_bool(c.F, v);
}

jit_value_t OperatorPrototypes::store_real_real(Compiler& c, jit_value_t a, jit_value_t b) {
	jit_insn_store(c.F, a, b);
	return b;
}

jit_value_t OperatorPrototypes::add_real_real(Compiler& c, jit_value_t a, jit_value_t b) {
	return jit_insn_add(c.F, a, b);
}
jit_value_t OperatorPrototypes::add_equals_real_real(Compiler& c, jit_value_t a, jit_value_t b) {
	jit_insn_store(c.F, a, jit_insn_add(c.F, a, b));
	return b;
}


}
