#include "Test.hpp"

void Test::test_classes() {

	header("Classes");

	success("null.class", "<class Null>");
	success("true.class", "<class Boolean>");
	success("false.class", "<class Boolean>");
	success("0.class", "<class Number>");
	success("12.class", "<class Number>");
	success("-5.class", "<class Number>");
	success("12.5234.class", "<class Number>");
	success("''.class", "<class String>");
	success("'salut'.class", "<class String>");
	success("[].class", "<class Array>");
	success("[1, 2, 3].class", "<class Array>");
	success("{}.class", "<class Object>");
	success("{a: 12, b: 'yo'}.class", "<class Object>");
	success("(-> 12).class", "<class Function>");
	success("(x, y -> x + y).class", "<class Function>");
	success("12.class.class", "<class Class>");
/*
	success("class A {} let a = new A", "{}");
	success("class A { let b = 2 } let a = new A", "{}");
	success("class A { let b = 2 } let a = new A a.b", "2");
	success("class A { let b = 2 } let a = new A a.class", "<class A>");
	success("class A {} let t = [A] let a = new t[0]", "{}");
	success("class A {} let a = new A let b = new a.class b.class", "<class A>");
*/

//	success("let my_map = [].map; my_map([1, 2, 3], x -> x ^ 2)", "[1, 4, 9]");
//	success("[].map == [].map", "true");
//	success("{}.map == {}.map", "true");
//	success("[].map == {}.map", "false");
//	success("let a = [].map; a == [].map", "true");
}
