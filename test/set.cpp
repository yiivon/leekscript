#include "Test.hpp"

void Test::test_set() {

	header("Set");
	section("Constructor");
	code("<>").equals("<>");
	code("<2, 1, 1>").equals("<1, 2>");
	code("<1.56, -2.312, 7.23>").equals("<-2.312, 1.56, 7.23>");
	code("<'2', '1', '1'>").equals("<'1', '2'>");
	code("<1l, 2l, 3l>").equals("<1, 2, 3>");

	/*
	 * Operators
	 */
	section("Set.to_bool()");
 	code("if <1, 2> then 5 else 12 end").equals("5");
	code("if <1.5> then 5 else 12 end").equals("5");
 	code("if <> then 5 else 12 end").equals("12");

	section("Set.operator ! ()");
	code("!<1, 2>").equals("false");
	code("!<1.5, 2.5>").equals("false");
	code("!<'a', 'b'>").equals("false");
	code("!<>").equals("true");

	section("Set.operator in ()");
	code("let s = <1, 2> 3 in s").equals("false");
	code("let s = <1, 2> 1 in s").equals("true");
	code("let s = <1.5, 2.5> 3.5 in s").equals("false");
	code("let s = <1.5, 2.5> 1.5 in s").equals("true");
	code("let s = <'a', 'b'> 'a' in s").equals("true");
	code("let s = <'a', 'b'> 12 in s").equals("false");

	section("Set.operator ==");
	code("<> == <>").equals("true");
	code("<1, 2, 3> == <1, 2, 3>").equals("true");
	code("<1, 2, 3> == <1, 2, 4>").equals("false");
	code("<'a', 'b', 'c'> == <'a', 'b', 'c'>").equals("true");
	code("<'a', 'b', 'c'> == <1, 2, 3>").equals("false");
	code("<1, 2, [3, ''][0]> == <1, 2, 3>").equals("true");
	code("<1.12, 2.12, [3.12, ''][0]> == <1.12, 2.12, 3.12>").equals("true");
	code("<1, 2, 3> == [1, 2, 3]").equals("false");

	section("Set.operator <");
	code("<> < <>").equals("false");
	code("<1> < <2>").equals("true");
	code("<2> < <1>").equals("false");
	code("<1, 2, 3> < <1, 2, 4>").equals("true");
	code("<2, 2, 3> < <1, 2, 3>").equals("false");
	code("<1.7, 2, 3.90> < <1.7, 2, 5.6>").equals("true");
	code("<'a', 'b', 'c'> < <'a', 'b', 'd'>").equals("true");
	code("<'a', 2, true> < <'a', 2, false>").equals("false");
	code("<1, 2, 3> < <1, 2, []>").equals("true");
	code("<1, 2.5, 3.5> < <1, 2.5, []>").equals("true");
	code("<1, 2, []> < <1, 2, 3>").equals("false");
	code("<1, 2, []> < <1, 2, 3.5>").equals("false");
	code("ptr(<1, 2>) < 6").equals("false");
	code("ptr(<1, 2>) < [1, 2]").equals("false");
	code("ptr(<1, 2>) < (x -> x)").equals("true");
	code("<1, 2> < <1, 2, 3>").equals("true");
	code("<1, 2, 3> < <1, 2>").equals("false");
	code("ptr(<1, 2, 3>) < ptr(<>)").equals("false");
	code("<1, 2, 3, 4> < <1, 2, 3, []>").equals("true");

	section("Set.operator +=");
	code("var s = <> s += 0 s").equals("<0>");
	code("var s = <> s += 0.5 s").equals("<0.5>");
	code("var s = <> s += 'a' s").equals("<'a'>");
	code("var s = <1> s += 0 s").equals("<0, 1>");
	code("var s = <1.5> s += 0.5 s").equals("<0.5, 1.5>");
	code("var s = <'b'> s += 'a' s").equals("<'a', 'b'>");
	code("var s = <1, 2> s += 'a' s").equals("<1, 2, 'a'>");
	code("var s = <'a', 'b'> s += 1 s").equals("<1, 'a', 'b'>");
	code("var s = <1, 2> s += <3, 4> s").equals("<1, 2, 3, 4>");
	code("var s = <1, 2> s += <3.2, 4.9> s").equals("<1, 2, 3.2, 4.9>");
	code("var s = <1, 2> s += <'a', 'b'> s").equals("<1, 2, 'a', 'b'>");
	code("var s = <1.5, 2.9> s += <3, 4> s").equals("<1.5, 2.9, 3, 4>");
	code("var s = <1.5, 2.9> s += <3.2, 4.9> s").equals("<1.5, 2.9, 3.2, 4.9>");
	code("var s = <1.5, 2.9> s += <'a', 'b'> s").equals("<1.5, 2.9, 'a', 'b'>");
	code("var s = <'a', 'b'> s += <3, 4> s").equals("<3, 4, 'a', 'b'>");
	code("var s = <'a', 'b'> s += <3.2, 4.9> s").equals("<3.2, 4.9, 'a', 'b'>");
	code("var s = <'a', 'b'> s += <'c', 'd'> s").equals("<'a', 'b', 'c', 'd'>");

	/*
	 * Methods
	 */
	section("Set.contains()");
	code("let s = <1, 2> s.contains(3)").equals("false");
	code("let s = <1, 2> s.contains(1)").equals("true");

	section("Set.clear()");
	code("let s = <1, 2> s.clear() s").equals("<>");
	code("let s = <'a', 'b'> s.clear() s").equals("<>");
	code("let s = <1.5, 2.5> s.clear() s").equals("<>");

	section("Set.erase()");
	code("let s = <1, 2> s.erase(3)").equals("false");
	code("let s = <1, 2> s.erase(1)").equals("true");
	code("let s = <1, 2, 3.3> s.erase(3.3)").equals("true");
	code("let s = <'a', 'b'> s.erase('a')").equals("true");
	code("let s = <'a', 'b'> s.erase('c')").equals("false");

	section("Set.insert()");
	code("let s = <1, 2> s.insert(3) s").equals("<1, 2, 3>");
	code("let s = <1, 2.5> s.insert(3.5) s").equals("<1, 2.5, 3.5>");
	code("let s = <'a', 'b'> s.insert('c') s").equals("<'a', 'b', 'c'>");
	code("<'a'>.insert('b')").equals("true");
	code("<'a'>.insert('a')").equals("false");

	section("Set.size()");
	code("<>.size()").equals("0");
	code("<1, 2, 3>.size()").equals("3");
	code("<1.6, 2.1, 3.75, 12.12>.size()").equals("4");
	code("<'éééé', null>.size()").equals("2");
	code("Set.size(<1, 2, 3>)").equals("3");
	code("Set.size(<1.6, 2.1, 3.75, 12.12>)").equals("4");
	code("Set.size(<'éééé', null>)").equals("2");

	section("Set clone()");
	code("let s = <1, 2, 3> [s]").equals("[<1, 2, 3>]");
}
