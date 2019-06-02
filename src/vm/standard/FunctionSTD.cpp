#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSFunction.hpp"
#include "../value/LSClosure.hpp"

namespace ls {

FunctionSTD::FunctionSTD(VM* vm) : Module(vm, "Function") {

	LSFunction::clazz = clazz;

	field("return", Type::clazz(), field_return);
	field("args", Type::array(Type::clazz()), field_args);

	constructor_({
		{Type::fun(Type::void_, {}), {Type::i8->pointer()}, (void*) LSFunction::constructor},
		{Type::closure(Type::void_, {}), {Type::i8->pointer()}, (void*) LSClosure::constructor},
	});

	// method("copy", {
	// 	{Type::FUNCTION, {Type::CONST_FUNCTION}, (void*) ValueSTD::copy}
	// });

	/** Internal **/
	method("call", {
		{Type::any, {Type::fun(Type::void_, {})}, (void*) &LSFunction::call}
	});
	method("get_capture", {
		{Type::any, {Type::closure(Type::void_, {})}, (void*) &LSClosure::get_capture}
	});
	method("get_capture_l", {
		{Type::any, {Type::closure(Type::void_, {})}, (void*) &LSClosure::get_capture_l}
	});
	method("add_capture", {
		{Type::void_, {Type::closure(Type::void_, {}), Type::any}, (void*) &LSClosure::add_capture}
	});
}

Compiler::value FunctionSTD::field_return(Compiler& c, Compiler::value function) {
	auto class_name = function.t->return_type()->class_name();
	if (!class_name.size()) class_name = "Value";
	return c.get_symbol(class_name, Type::clazz(class_name));
}

Compiler::value FunctionSTD::field_args(Compiler& c, Compiler::value function) {
	std::vector<Compiler::value> args;
	for (const auto& arg : function.t->arguments()) {
		auto class_name = arg->class_name();
		if (!class_name.size()) class_name = "Value";
		args.push_back(c.get_symbol(class_name, Type::clazz(class_name)));
	}
	return c.new_array(Type::array(Type::clazz()), args);
}

}
