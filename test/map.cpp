#include "Test.hpp"

void Test::test_map() {

	header("Map");

	success("[:]", "[:]");

	success("[1 : 1 2 : 2]", "[1 : 1 2 : 2]");
	success("[1 : 1 2 : '2']", "[1 : 1 2 : '2']");

	success("let x = [1 : 1 1 : 2 1 : 3] x.size()", "1");
	success("let x = [1 : 1 1 : 2 2 : '3'] x.size()", "2");

	success("let x = [1 : 1] x.insert(2, 2)", "[1 : 1 2 : 2]");
	success("let x = ['a' : 'a'] x.insert(2, 2)", "[2 : 2 'a' : 'a']");
	success("let x = [1 : 'a'] x.insert(2, 3)", "[1 : 'a' 2 : 3]");
	success("let x = ['a' : 1] x.insert(2, 3)", "[2 : 3 'a' : 1]");

	success("let x = [1 : 1] x.clear()", "[:]");
	success("let x = ['a' : 'a'] x.clear()", "[:]");

	success("let x = [1 : 1] x.erase(1)", "[:]");
	success("let x = ['a' : 'a'] x.erase('a')", "[:]");
	success("let x = ['a' : 'a'] x.erase('b')", "['a' : 'a']");
	success("let x = ['a' : 1] x.erase(3.14)", "['a' : 1]");

	success("['a':'b'] == [1:1]", "false");
	//success("let x = ['a' : 'b'] let y = [1 : 1] x.clear() == y.clear()", "true");
}
