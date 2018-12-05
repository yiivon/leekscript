#include "ObjectSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSObject.hpp"
#include "../value/LSNumber.hpp"

namespace ls {

LSObject ObjectSTD::readonly;
LSNumber* ObjectSTD::readonly_value = LSNumber::get(12);

ObjectSTD::ObjectSTD() : Module("Object") {

	LSObject::object_class = clazz;

	readonly.addField("v", readonly_value);
	readonly.readonly = true;
	readonly.native = true;

	static_field("readonly", Type::OBJECT, [&](Compiler& c) {
		return c.new_pointer(&ObjectSTD::readonly);
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::OBJECT, Type::ANY, Type::BOOLEAN, (void*) &LSObject::in, {}, Method::NATIVE},
		{Type::OBJECT, Type::NUMBER, Type::BOOLEAN, (void*) &in_any}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::OBJECT, {Type::OBJECT}, (void*) &ValueSTD::copy}
	});
	Type map_fun_type = Type::fun(Type::ANY, {Type::ANY});
	auto map_fun = &LSObject::ls_map<LSFunction*>;
	method("map", {
		{Type::OBJECT, {Type::OBJECT, map_fun_type}, (void*) map_fun, Method::NATIVE}
	});
	method("keys", {
		{Type::STRING_ARRAY, {Type::OBJECT}, (void*) &LSObject::ls_get_keys, Method::NATIVE}
	});
	method("values", {
		{Type::PTR_ARRAY, {Type::OBJECT}, (void*) &LSObject::ls_get_values, Method::NATIVE}
	});
}

Compiler::value ObjectSTD::in_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::ANY, {args[0], c.insn_to_any(args[1])}, (void*) +[](LSValue* x, LSValue* y) {
		return x->in(y);
	});
}

}
