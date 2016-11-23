#include "ValueSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"

namespace ls {

ValueSTD::ValueSTD() : Module("Value") {

	operator_("instanceof", {
		{Type::POINTER, Type::CLASS, Type::BOOLEAN, (void*) &ValueSTD::instanceof_ptr}
	});
	operator_("and", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::and_value_value, Method::NATIVE}
	});
	operator_("or", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::or_value_value, Method::NATIVE}
	});
}

bool ValueSTD::instanceof_ptr(LSValue* x, LSClass* c) {
	bool r = ((LSClass*) x->getClass())->name == c->name;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(c);
	return r;
}

Compiler::value ValueSTD::and_value_value(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_and(
		c.insn_to_bool(args[0]),
		c.insn_to_bool(args[1])
	);
}
Compiler::value ValueSTD::or_value_value(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_or(
		c.insn_to_bool(args[0]),
		c.insn_to_bool(args[1])
	);
}

}
