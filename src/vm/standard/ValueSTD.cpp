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
	method("string", {
		{Type::UNKNOWN, Type::STRING, {}, (void*) &ValueSTD::to_string, Method::NATIVE}
	});
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

Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t == Type::INTEGER) {
		return c.insn_call(Type::STRING, args, +[](int v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t == Type::LONG) {
		return c.insn_call(Type::STRING, args, +[](long v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t == Type::REAL) {
		return c.insn_call(Type::STRING, args, +[](double v) {
			return new LSString(LSNumber::print(v));
		});
	}
	if (args[0].t == Type::BOOLEAN) {
		return c.insn_call(Type::STRING, args, +[](bool b) {
			return new LSString(b ? "true" : "false");
		});
	}
	if (args[0].t.nature == Nature::POINTER) {
		return c.insn_call(Type::STRING, args, (void*) &LSValue::ls_json);
	}
	std::cout << "Type non supporté !" << std::endl;
	throw new std::exception();
	return {nullptr, Type::VOID};
}
}

}
