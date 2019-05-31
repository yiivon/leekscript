#include "ClassSTD.hpp"
#include "../value/LSClass.hpp"

namespace ls {

ClassSTD::ClassSTD(VM* vm) : Module(vm, "Class") {

	LSClass::clazz = clazz;

	field("name", Type::string());

	constructor_({
		{Type::clazz(), {Type::i8().pointer()}, (void*) LSClass::constructor},
	});

	/** Internal **/
	method("add_field", {
		{{}, {Type::clazz(), Type::i8().pointer(), Type::any()}, (void*) add_field}
	});
}

void ClassSTD::add_field(LSClass* clazz, char* field_name, LSValue* default_value) {
	clazz->addField(field_name, Type::any(), nullptr);
	clazz->fields.at(field_name).default_value = default_value;
}

}
