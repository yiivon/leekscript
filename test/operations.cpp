#include "Test.hpp"

void Test::test_operations() {

	header("Operations");

	ops("2", 0);
	ops("2 + 2", 1);
	ops("2 * 3 + 4 * 5", 3);
	ops("3 < 5", 1);
	ops("[]", 1);
	ops("[1, 2, 3]", 4);
	ops("[1, 2, 3, 4, 5, 6, 7, 8, 9]", 10);
	ops("[1][0]", 4);

	ops("x -> x", 1);
	ops("(x -> x + 1)(12)", 3);
}
