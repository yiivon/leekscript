#include "Test.hpp"

void Test::test_set() {

	header("Set");

	success("<>", "<>");

	success("<2, 1, 1>", "<1, 2>");
	success("<'2', '1', '1'>", "<'1', '2'>");
}
