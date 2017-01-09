#include "Test.hpp"

void Test::test_set() {

	header("Set");
	section("Constructor");
	code("<>").equals("<>");
	code("<2, 1, 1>").equals("<1, 2>");
	code("<1.56, -2.312, 7.23>").equals("<-2.312, 1.56, 7.23>");
	code("<'2', '1', '1'>").equals("<'1', '2'>");

	section("Set.operator in ()");
	code("let s = <1, 2> 3 in s").equals("false");
	code("let s = <1, 2> 1 in s").equals("true");
	section("Set.contains()");
	code("let s = <1, 2> s.contains(3)").equals("false");
	code("let s = <1, 2> s.contains(1)").equals("true");

	section("Set.clear()");
	code("let s = <1, 2> s.clear() s").equals("<>");

	section("Set.erase()");
	code("let s = <1, 2> s.erase(3)").equals("false");
	code("let s = <1, 2> s.erase(1)").equals("true");

	section("Set.insert()");
	code("let s = <1, 2> s.insert(3) s").equals("<1, 2, 3>");

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

	// Type changes
	// code("let s = <1, 2> s += 'a' s").equals("<1, 2, 'a'>");
}
