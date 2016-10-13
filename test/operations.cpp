#include "Test.hpp"

void Test::test_operations() {

	header("Operations");

	code("2").operations(0);
	code("2 + 2").operations(1);
	code("2 * 3 + 4 * 5").operations(3);
	code("3 < 5").operations(1);
	code("[]").operations(1);
	code("[1, 2, 3]").operations(4);
	code("[1, 2, 3, 4, 5, 6, 7, 8, 9]").operations(10);
	code("[1][0]").operations(4);

	code("x -> x").operations(1);
	code("(x -> x + 1)(12)").operations(3);
}
