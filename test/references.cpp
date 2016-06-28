#include "Test.hpp"

void Test::test_references() {

	header("References");

	test("let a = 2 let b = a a = 4 b", "2");
	test("let a = 2 let b = @a a = 5 b", "5");
	test("let a = 2 let b = @a b = 6 a", "6");
	test("let a = 2 let b = @a let c = @b a = 7 c", "7");
	test("let a = 2 let b = @a let c = @b c = 8 a", "8");

	test("let a = 'hello' let b = @a a = 'world' b", "'world'");
	test("let a = true let b = @a b = false b", "false");
	test("let a = true let b = @a b = false a", "false");

	// Type change from int to string
	//test("let a = 12 let b = @a b = 'salut' a", "'salut'");
	test("let a = [1, 2, 3] let b = @a b = 'salut' a", "'salut'");


	test("let v = 10 let a = [v] a[0]++ v", "10");
//	test("let v = 10 let a = [@v] a[0]++ v", "11");
}
