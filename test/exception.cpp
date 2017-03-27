#include "Test.hpp"

void Test::test_exceptions() {

	header("Exceptions");
	code("throw 2").ops_limit(1000).exception(ls::VM::Exception::OPERATION_LIMIT_EXCEEDED);

	code("throw").exception(ls::VM::Exception::EXCEPTION);
	code("12").exception(ls::VM::Exception::NO_EXCEPTION);

	code("var a = 12m; throw").exception(ls::VM::Exception::EXCEPTION);
	code("var a = 12m + 5m; throw").exception(ls::VM::Exception::EXCEPTION);

	section("Exceptions - Avoid leaks");
	code("var x = 'hello' [][0]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("var x = 'hello' throw").exception(ls::VM::Exception::EXCEPTION);
	code("let f = { var x = 'hello' [][0] } f()").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let f = -> { var x = 'hello' throw } f()").exception(ls::VM::Exception::EXCEPTION);
}
