#include "Test.hpp"

void Test::test_map() {

	header("Map");

	code("[:]").equals("[:]");

	code("[1 : 1 2 : 2]").equals("[1 : 1 2 : 2]");
	code("[1 : 1 2 : '2']").equals("[1 : 1 2 : '2']");
	code("['1' : '1' '1' : '2' '1' : '3']").equals("['1' : '1']");

	code("let x = [1 : 1 1 : 2 1 : 3] x.size()").equals("1");
	code("let x = [1 : 1 1 : 2 2 : '3'] x.size()").equals("2");

	code("let x = [1 : 1] x.insert(2, 2)").equals("true");
	code("let x = ['a' : 'a'] x.insert(2, 2) x").equals("[2 : 2 'a' : 'a']");
	code("let x = [1 : 'a'] x.insert(2, 3) x").equals("[1 : 'a' 2 : 3]");
	code("let x = ['a' : 1] x.insert(2, 3) x").equals("[2 : 3 'a' : 1]");
	code("let x = ['a' : 1] x.insert('a', 3)").equals("false");

	code("let x = [1 : 1] x.clear()").equals("[:]");
	code("let x = ['a' : 'a'] x.clear()").equals("[:]");

	code("let x = [1 : 1] x.erase(1)").equals("true");
	code("let x = ['a' : 'a'] x.erase('a') x").equals("[:]");
	code("let x = ['a' : 'a'] x.erase('b') x").equals("['a' : 'a']");
	code("let x = ['a' : 1] x.erase(3.14) x").equals("['a' : 1]");

	code("let x = [1 : 1] x.look(1,0)").equals("1");
	code("let x = ['a' : 'a'] x.look('a','b')").equals("'a'");
	code("let x = ['a' : 'a'] x.look('b','b')").equals("'b'");
	code("let x = ['a' : 1] x.look(3.14,'a')").semantic_error( ls::SemanticError::METHOD_NOT_FOUND, "look");

	code("['a':'b'] == [1:1]").equals("false");
	code("let x = ['a' : 'b'] let y = [1 : 1] x.clear() == y.clear()").equals("true");

	code("['a':1 'b':2] < ['a':1 'b':3]").equals("true");
	code("[1:1 2:0] < [1:1 2:true]").equals("false");
}
