#include "FunctionSTD.hpp"
#include "../value/LSFunction.hpp"

namespace ls {

FunctionSTD::FunctionSTD() : Module("Function") {

	LSFunction<LSValue*>::clazz = clazz;
}


}
