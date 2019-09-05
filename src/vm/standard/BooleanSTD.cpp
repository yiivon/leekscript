#include "BooleanSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include "../../type/Type.hpp"

namespace ls {

BooleanSTD::BooleanSTD(VM* vm) : Module(vm, "Boolean") {

	LSBoolean::clazz = clazz.get();

	operator_("+", {
		{Type::const_boolean, Type::const_string, Type::tmp_string, (void*) add},
		{Type::const_boolean, Type::tmp_string, Type::tmp_string, (void*) add_tmp},
		{Type::const_boolean, Type::const_boolean, Type::integer, add_bool},
		{Type::const_boolean, Type::const_real, Type::real, add_bool},
		{Type::const_boolean, Type::const_integer, Type::integer, add_bool}
	});

	operator_("-", {
		{Type::const_boolean, Type::const_boolean, Type::integer, sub_bool},
		{Type::const_boolean, Type::const_real, Type::real, sub_bool},
		{Type::const_boolean, Type::const_integer, Type::integer, sub_bool}
	});

	operator_("*", {
		{Type::const_boolean, Type::const_boolean, Type::integer, mul_bool},
		{Type::const_boolean, Type::const_real, Type::real, mul_bool},
		{Type::const_boolean, Type::const_integer, Type::integer, mul_bool}
	});

	method("compare", {
		{Type::any, {Type::const_any, Type::const_any}, (void*) compare_ptr_ptr_ptr},
		{Type::integer, {Type::const_boolean, Type::const_any}, compare_val_val}
	});

	/** Internal **/
	method("to_string", {
		{Type::tmp_string, {Type::boolean}, (void*) to_string}
	});
}

LSString* BooleanSTD::add(int boolean, LSString* string) {
	return new LSString((boolean ? "true" : "false") + *string);
}

LSString* BooleanSTD::add_tmp(int boolean, LSString* string) {
	(*string).insert(0, (boolean ? "true" : "false"));
	return string;
}

Compiler::value BooleanSTD::add_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_add(args[0], args[1]);
}

Compiler::value BooleanSTD::sub_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_sub(args[0], args[1]);
}

Compiler::value BooleanSTD::mul_bool(Compiler& c, std::vector<Compiler::value> args, int) {
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

Compiler::value BooleanSTD::compare_val_val(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_cmpl(args[0], args[1]);
}

LSValue* BooleanSTD::to_string(bool b) {
	return new LSString(b ? "true" : "false");
}

}
