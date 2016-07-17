#include "Test.hpp"

void Test::test_objects() {

	header("Objects");

	success("{}", "{}");
	success("{a: 12}", "{a: 12}");
	success("{a: 12, b: 5}", "{a: 12, b: 5}");
	success("{a: {}, b: []}", "{a: {}, b: []}");

	success("let a = {}", "{}");
	success("let a = {b: 12, c: 5} a", "{b: 12, c: 5}");
	success("let a = {b: 12, c: 5} a.b", "12");
	//success("let a = {b: 12, c: 5} a.b *= 10", "120");
	success("let a = {a: 32, b: 'toto', c: false}; |a|", "3");

	success("{}.keys()", "[]");
	success("{a: 5, b: 'toto', c: true, d: -> 5}.keys()", "['a', 'b', 'c', 'd']");

	success("{}.values()", "[]");
	success("{a: 5, b: 'toto', c: true, d: -> 5}.values()", "[5, 'toto', true, <function>]");

//	success("var f = obj -> obj.a [f(12), f({a: 'yo'})]", "[null, 'yo']");

	success("{a: 12 b: 5}", "{a: 12, b: 5}");
	success("{a: 12 - 2 yo: -6}", "{a: 10, yo: -6}");
	success("{a: 12 b: 'yo' c: true d: [1 2 3]}", "{a: 12, b: 'yo', c: true, d: [1, 2, 3]}");
}
