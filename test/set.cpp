#include "Test.hpp"

void Test::test_set() {

	header("Set");

	code("<>").equals("<>");
	code("<2, 1, 1>").equals("<1, 2>");
	code("<'2', '1', '1'>").equals("<'1', '2'>");

	code("let s = <1, 2> s.insert(3) s").equals("<1, 2, 3>");
	code("let s = <1, 2> s.clear() s").equals("<>");
	code("let s = <1, 2> s.erase(3)").equals("false");
	code("let s = <1, 2> s.erase(1)").equals("true");
	code("let s = <1, 2> s.contains(3)").equals("false");
	code("let s = <1, 2> s.contains(1)").equals("true");
	code("let s = <1, 2> 3 in s").equals("false");
	code("let s = <1, 2> 1 in s").equals("true");

	// Type changes
	// code("let s = <1, 2> s += 'a' s").equals("<1, 2, 'a'>");
}
