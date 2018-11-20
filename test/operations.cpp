#include "Test.hpp"

void Test::test_operations() {

	header("Operations");
	DISABLED_code("2").operations(0);
	DISABLED_code("2 + 2").operations(1);
	DISABLED_code("2 * 3 + 4 * 5").operations(3);
	DISABLED_code("3 < 5").operations(1);
	DISABLED_code("[]").operations(1);
	DISABLED_code("[1, 2, 3]").operations(4);
	DISABLED_code("[1, 2, 3, 4, 5, 6, 7, 8, 9]").operations(10);
	DISABLED_code("[1][0]").operations(4);
	DISABLED_code("x -> x").operations(0);
	DISABLED_code("(x -> x + 1)(12)").operations(3);

	section("GMP operations");
	DISABLED_code("12345m ** 12").operations(169);
	DISABLED_code("12345m ** 120").operations(1681);

	section("Operation limit exceeded");
	DISABLED_code("while true {}").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
	DISABLED_code("for ;; {}").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
}
