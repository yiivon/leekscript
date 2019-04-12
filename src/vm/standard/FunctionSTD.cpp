#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSFunction.hpp"

namespace ls {

FunctionSTD::FunctionSTD() : Module("Function") {

	LSFunction::clazz = clazz;

	field("return", Type::clazz());
	field("args", Type::array());

	constructor_({
		{Type::fun({}, {}), {Type::fun({}, {})}, (void*) &LSFunction::constructor, Method::NATIVE}
	});

	// method("copy", {
	// 	{Type::FUNCTION, {Type::CONST_FUNCTION}, (void*) &ValueSTD::copy}
	// });
}

}
