#include "Test.hpp"

void Test::test_references() {

	header("References");
	section("Basic references with integers");
	code("var a = 2; let b = a; a = 4; b").equals("2");
	code("var a = 2; let b = @a; a = 5; b").equals("5");
	code("var a = 2; var b = @a; b = 6; a").equals("6");
	code("var a = 2; let b = @a; let c = @b; a = 7; c").equals("7");
	code("var a = 2; let b = @a; var c = @b; c = 8; a").equals("8");

	section("Reference in expression");
	code("var a = 2; let b = (@a + 1); a = 5; b").equals("3");

	section("Reference in declaration");
	code("var a = 2, b = @a; a = 55; b").equals("55");
	code("var a = 2, b = @a; b = 56; a").equals("56");

	section("Basic references with strings");
	code("var a = 'hello'; var b = @a; a = 'world'; b").equals("'world'");
	code("var a = true; var b = @a; b = false; b").equals("false");
	code("var a = true; var b = @a; b = false; a").equals("false");

	section("Array reference");
	code("var a = [1, 2, 3]; var b = @a; b[1] = 12; a").equals("[1, 12, 3]");
	// code("var a = [1, 2, 3]; var b = @a; b[1]++; a").equals("[1, 3, 3]");
	code("var a = [1, 2, 3]; var b = @a; b.clear(); a").equals("[]");
	code("var a = [1, 2, 3]; var b = @a; b.push(4); a").equals("[1, 2, 3, 4]");
	code("var a = ['a', 'b', 'c']; var b = @a; b.push([12]); a").equals("['a', 'b', 'c', [12]]");

	section("Type changes");
	// code("var a = 12; var b = @a; b = 'salut'; a").equals("'salut'");
	code("var a = [1, 2, 3]; var b = @a; b = 'salut'; a").equals("'salut'");

	section("Reference in array");
	code("var v = 10; var a = [v]; a[0]++; v").equals("10");
	// code("var v = 10; var a = [@v]; a[0]++; v").equals("11");
}
