#include "Test.hpp"

void Test::test_objects() {

	header("Objects");

	success("let a = {}", "{}");
	success("let a = {b: 12, c: 5} a", "{b: 12, c: 5}");
	success("let a = {b: 12, c: 5} a.b", "12");
	success("let a = {b: 12, c: 5} a.b *= 10", "120");
	success("let a = {a: 32, b: 'toto', c: false} |a|", "3");

	success("{}.keys()", "[]");
	success("{a: 5, b: 'toto', c: true, d: -> 5}.keys()", "['a', 'b', 'c', 'd']");

	success("{}.values()", "[]");
	success("{a: 5, b: 'toto', c: true, d: -> 5}.values()", "[5, 'toto', true, <function>]");
}
