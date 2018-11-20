#include "Test.hpp"

void Test::test_exceptions() {

	header("Exceptions");
	code("throw 2").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);

	code("throw").exception(ls::vm::Exception::EXCEPTION);
	code("12").exception(ls::vm::Exception::NO_EXCEPTION);

	code("var a = 12m; throw").exception(ls::vm::Exception::EXCEPTION);
	code("var a = 12m + 5m; throw").exception(ls::vm::Exception::EXCEPTION);

	section("Exceptions - Avoid leaks");
	DISABLED_code("var x = 'hello' [][0]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	DISABLED_code("var x = 'hello' throw").exception(ls::vm::Exception::EXCEPTION);
	DISABLED_code("let f = -> { var x = 'hello' [][0] } f()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS, {
		{"f", 1}, {"main", 1}
	});
	DISABLED_code("let f = -> { var x = 'hello' throw } f()").exception(ls::vm::Exception::EXCEPTION, {
		{"f", 1}, {"main", 1}
	});

	section("Catch-else operator");
	DISABLED_code("2 !? 5").equals("2");
	DISABLED_code("{ throw } !? 5").equals("5");
	DISABLED_code("[][0] !? 6").equals("6");
	DISABLED_code("(2 \\ 0) !? 7").equals("7");
	DISABLED_code("([][0] !? 12) + ([][0] !? 5)").equals("17");
	DISABLED_code("([0][1] !? 12) + ([0][1] !? 5)").equals("17");
	DISABLED_code("let f = -> { { throw } !? 5 } f()").equals("5");
	DISABLED_code("let f = -> { (2 \\ 0) !? 6 } f()").equals("6");
	DISABLED_code("let f = -> { var s = 'leak?' { [][0] } !? 7 } f()").equals("7");
	// TODO a leak in the inner block
	DISABLED_code("let f = -> { var s = 'leak1' { var s = 'leak2' [][0] } !? 8 } f()").equals("8");

	section("Stacktraces");
	DISABLED_file("test/code/exception/exception_1.leek").exception(ls::vm::Exception::DIVISION_BY_ZERO, {
		{"crash", 2}, {"power", 7}, {"will_crash", 12}, {"do_something", 17}, {"main", 20}
	});
	DISABLED_file("test/code/exception/exception_2.leek").exception(ls::vm::Exception::NO_SUCH_OPERATOR, {
		{"crash", 2}, {"power", 7}, {"will_crash", 12}, {"do_something", 17}, {"main", 20}
	});
	DISABLED_file("test/code/exception/exception_3.leek").exception(ls::vm::Exception::EXCEPTION, {
		{"crash", 2}, {"power", 7}, {"will_crash", 12}, {"do_something", 17}, {"main", 20}
	});
	DISABLED_file("test/code/exception/exception_4.leek").exception(ls::vm::Exception::NUMBER_OVERFLOW, {
		{"crash", 2}, {"power", 7}, {"will_crash", 12}, {"do_something", 17}, {"main", 20}
	});
	DISABLED_file("test/code/exception/exception_5.leek").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS, {
		{"crash", 3}, {"power", 8}, {"will_crash", 13}, {"do_something", 18}, {"main", 21}
	});
}
