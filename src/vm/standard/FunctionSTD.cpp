#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSFunction.hpp"

namespace ls {

FunctionSTD::FunctionSTD() : Module("Function") {

	LSFunction::clazz = clazz;

	field("return", Type::CLASS);
	field("args", Type::PTR_ARRAY);

	method("copy", {
		{Type::FUNCTION, {Type::CONST_FUNCTION}, (void*) &ValueSTD::copy}
	});
}

}
