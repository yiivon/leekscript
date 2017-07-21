#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSFunction.hpp"

namespace ls {

FunctionSTD::FunctionSTD() : Module("Function") {

	LSFunction::clazz = clazz;

	field("return", Type::CLASS);
	field("args", Type::PTR_ARRAY);

	method("copy", {
		{Type::FUNCTION_P, {Type::CONST_FUNCTION_P}, (void*) &ValueSTD::copy}
	});
}

}
