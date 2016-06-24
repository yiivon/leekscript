#include "Test.hpp"

void Test::test_operations() {

	ops("2", 0);
	ops("2 + 2", 1);
	ops("2 * 3 + 4 * 5", 3);
	ops("3 < 5", 1);
}
