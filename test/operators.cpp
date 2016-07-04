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

	/*
	 * Swap
	 */
	header("Swap");
	success("let a = 2 let b = 5 a <=> b [a, b]", "[5, 2]");
//	success("let a = [1, 2, 3, 4] a[0] <=> a[3] a", "[4, 2, 3, 1]");
	success("let a = 12 let b = 5 let s = a <=> b", "5");

	/*
	a ~~ b => (a ~~ b)
	a ~ ~b => a ~ (~b)
	~~a => ~(~a)
	a; ~~b
	*/
	/*
	var dist = [...]
	dist ~*= 5
	var newdist = dist ~* 5
	*/
}
