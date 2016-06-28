#include "Test.hpp"

void Test::test_operators() {

	header("Operator xor");

	test("true xor true", "false");
	test("true xor false", "true");
	test("false xor true", "true");
	test("false xor false", "false");

	test("12 xor 5", "false");
	test("12 xor 0", "true");
	test("false xor 99", "true");

	test("'salut' xor 5", "false");
	test("'salut' xor 'yo'", "false");
	test("'salut' xor false", "true");
	test("'salut' xor 0", "true");
	test("'salut' xor ''", "true");

	test("[] xor ''", "false");
	test("[] xor 777", "true");
	test("[1, 2, 3] xor [4, 5, 6]", "false");
	test("[] xor [4, 5, 6]", "true");
}
