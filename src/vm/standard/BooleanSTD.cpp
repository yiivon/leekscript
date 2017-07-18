#include "BooleanSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"

namespace ls {

BooleanSTD::BooleanSTD() : Module("Boolean") {

	LSBoolean::clazz = clazz;

	operator_("+", {
		{Type::CONST_BOOLEAN, Type::CONST_STRING, Type::STRING_TMP, (void*) &BooleanSTD::add, Method::NATIVE},
		{Type::CONST_BOOLEAN, Type::STRING_TMP, Type::STRING_TMP, (void*) &BooleanSTD::add_tmp, Method::NATIVE},
		{Type::CONST_BOOLEAN, Type::CONST_BOOLEAN, Type::INTEGER, (void*) &BooleanSTD::add_bool},
		{Type::CONST_BOOLEAN, Type::CONST_NUMBER, Type::REAL, (void*) &BooleanSTD::add_bool}
	});

	operator_("-", {
		{Type::CONST_BOOLEAN, Type::CONST_BOOLEAN, Type::INTEGER, (void*) &BooleanSTD::sub_bool},
		{Type::CONST_BOOLEAN, Type::CONST_NUMBER, Type::REAL, (void*) &BooleanSTD::sub_bool}
	});

	operator_("*", {
		{Type::CONST_BOOLEAN, Type::CONST_BOOLEAN, Type::INTEGER, (void*) &BooleanSTD::mul_bool},
		{Type::CONST_BOOLEAN, Type::CONST_NUMBER, Type::REAL, (void*) &BooleanSTD::mul_bool}
	});

	method("compare", {
		{Type::POINTER, {Type::CONST_POINTER, Type::CONST_POINTER}, (void*) &BooleanSTD::compare_ptr_ptr_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_POINTER, Type::CONST_POINTER}, (void*) &BooleanSTD::compare_ptr_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_BOOLEAN, Type::CONST_BOOLEAN}, (void*) &BooleanSTD::compare_val_val},
	});
}

LSString* BooleanSTD::add(int boolean, LSString* string) {
	return new LSString((boolean ? "true" : "false") + *string);
}

LSString* BooleanSTD::add_tmp(int boolean, LSString* string) {
	(*string).insert(0, (boolean ? "true" : "false"));
	return string;
}

Compiler::value BooleanSTD::add_bool(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_add(args[0], args[1]);
}

Compiler::value BooleanSTD::sub_bool(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_sub(args[0], args[1]);
}

Compiler::value BooleanSTD::mul_bool(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_mul(args[0], args[1]);
}

int BooleanSTD::compare_ptr_ptr(LSBoolean* a, LSBoolean* b) {
	int res = 0;
	if (a->value) {
		if (not b->value) res = 1;
	} else {
		if (b->value) res = -1;
	}
	LSValue::delete_temporary(a);
	LSValue::delete_temporary(b);
	return res;
}

LSValue* BooleanSTD::compare_ptr_ptr_ptr(LSBoolean* a, LSBoolean* b) {
	return LSNumber::get(compare_ptr_ptr(a, b));
}

Compiler::value BooleanSTD::compare_val_val(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_cmpl(c.F, args[0].v, args[1].v), Type::INTEGER};
}

}
