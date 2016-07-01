#include "Test.hpp"

void Test::test_operators() {

	header("Operator xor");

	success("true xor true", "false");
	success("true xor false", "true");
	success("false xor true", "true");
	success("false xor false", "false");

	success("12 xor 5", "false");
	success("12 xor 0", "true");
	success("false xor 99", "true");

	success("'salut' xor 5", "false");
	success("'salut' xor 'yo'", "false");
	success("'salut' xor false", "true");
	success("'salut' xor 0", "true");
	success("'salut' xor ''", "true");

	success("[] xor ''", "false");
	success("[] xor 777", "true");
	success("[1, 2, 3] xor [4, 5, 6]", "false");
	success("[] xor [4, 5, 6]", "true");
}
