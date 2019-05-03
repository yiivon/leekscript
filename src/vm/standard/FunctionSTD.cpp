#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSFunction.hpp"
#include "../value/LSClosure.hpp"

namespace ls {

FunctionSTD::FunctionSTD() : Module("Function") {

	LSFunction::clazz = clazz;

	field("return", Type::clazz());
	field("args", Type::array());

	constructor_({
		{Type::fun({}, {}), {Type::i8().pointer()}, (void*) &LSFunction::constructor, Method::NATIVE},
		{Type::closure({}, {}), {Type::i8().pointer()}, (void*) &LSClosure::constructor, Method::NATIVE},
	});

	// method("copy", {
	// 	{Type::FUNCTION, {Type::CONST_FUNCTION}, (void*) &ValueSTD::copy}
	// });

	/** Internal **/
	method("call", {
		{Type::any(), {Type::fun({}, {})}, (void*) &LSFunction::call, Method::NATIVE}
	});
}

}
