#include "Test.hpp"

void Test::test_operators() {

	header("Operator =");

	//success("let a a = 2", "2");
	//success("let a a = 2 a", "2");
	sem_err("2 = 2", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("'hello' = 2", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("[] = 2", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("true = []", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("null = x -> x", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("{} = 2.class", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");

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
	success("let a = 12 let b = 5 let s = a <=> b s", "5");

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

	res ?? 12 // res ? res : 12
	*/
}
