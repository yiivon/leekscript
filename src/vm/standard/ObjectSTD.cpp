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
		{Type::OBJECT, Type::POINTER, Type::BOOLEAN, (void*) &LSObject::in, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::OBJECT, {Type::OBJECT}, (void*) &ValueSTD::copy}
	});
	Type map_fun_type = Type::FUNCTION_P;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);
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

}
