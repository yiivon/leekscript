#include "Test.hpp"

void Test::test_exceptions() {

	header("Exceptions");
	code("throw 2").ops_limit(1000).exception(ls::VM::Exception::OPERATION_LIMIT_EXCEEDED);

	code("throw").exception(ls::VM::Exception::EXCEPTION);
}
