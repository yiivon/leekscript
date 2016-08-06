#include "Test.hpp"

void Test::test_map() {

	header("Map");

	success("[1 : 1 2 : 2]", "[1 : 1 2 : 2]");
	success("[1 : 1 2 : '2']", "[1 : 1 2 : '2']");

	//success("let x = [1 : 1] x.clear()", "[]");
	success("let x = [1 : 1] x.insert(2, 2)", "[1 : 1 2 : 2]");
	success("let x = [1 : 1 1 : 2 1 : 3] x.size()", "1");
}
