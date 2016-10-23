#include "Test.hpp"

void Test::test_map() {

	header("Map");

	section("Constructor");
	code("[:]").equals("[:]");
	code("[1 : 1 2 : 2]").equals("[1 : 1 2 : 2]");
	code("[1 : 1 2 : '2']").equals("[1 : 1 2 : '2']");
	code("['1' : '1' '1' : '2' '1' : '3']").equals("['1' : '1']");

	section("Map.size()");
	code("let x = [1 : 1 1 : 2 1 : 3] x.size()").equals("1");
	code("let x = [1 : 1 1 : 2 2 : '3'] x.size()").equals("2");

	section("Map.insert()");
	code("let x = [1 : 1] x.insert(2, 2)").equals("true");
	code("let x = ['a' : 'a'] x.insert(2, 2) x").equals("[2 : 2 'a' : 'a']");
	code("let x = [1 : 'a'] x.insert(2, 3) x").equals("[1 : 'a' 2 : 3]");
	code("let x = ['a' : 1] x.insert(2, 3) x").equals("[2 : 3 'a' : 1]");
	code("let x = ['a' : 1] x.insert('a', 3)").equals("false");

	section("Map.clear()");
	code("let x = [1 : 1] x.clear()").equals("[:]");
	code("let x = ['a' : 'a'] x.clear()").equals("[:]");

	section("Map.erase()");
	code("let x = [1 : 1] x.erase(1)").equals("true");
	code("let x = ['a' : 'a'] x.erase('a') x").equals("[:]");
	code("let x = ['a' : 'a'] x.erase('b') x").equals("['a' : 'a']");
	code("let x = ['a' : 1] x.erase(3.14) x").equals("['a' : 1]");

	section("Map.look()");
	code("let x = [1 : 1] x.look(1,0)").equals("1");
	code("let x = ['a' : 'a'] x.look('a','b')").equals("'a'");
	code("let x = ['a' : 'a'] x.look('b','b')").equals("'b'");
	code("let x = ['a' : 1] x.look(3.14,'a')").semantic_error( ls::SemanticError::METHOD_NOT_FOUND, ls::Type::PTR_INT_MAP.toString() + ".look(" + ls::Type::FLOAT.toString() + ", " + ls::Type::STRING.toString() + ")");

	section("Map.operator ==");
	code("['a':'b'] == [1:1]").equals("false");
	//code("let x = ['a' : 'b'] let y = [1 : 1] x.clear() == y.clear()").equals("true");

	section("Map.operator <");
	//code("['a':1 'b':2] < ['a':1 'b':3]").equals("true");
	code("[1:1 2:0] < [1:1 2:true]").equals("false");

	section("Map.operator in");
	code("let m = ['salut': 12] 'salut' in m").equals("true");
	code("let m = ['salut': 12] 'salum' in m").equals("false");

	section("Map.operator []");
	code("let m = [5: 12] m[5]").equals("12");
	code("let m = [5: 12.5] m[5]").equals("12.5");
	code("let m = [5.5: 12] m[5.5]").equals("12");
	code("let m = [5.5: 12.5] m[5.5]").equals("12.5");
	code("let m = ['salut': 12] m['salut']").equals("12");
	code("let m = ['salut': 12.5] m['salut']").equals("12.5");
	code("let m = ['salut': 'yolo'] m['salut']").equals("'yolo'");

	section("Map.operator [] left-value");
	code("let m = ['salut': 12] m['salut'] = 13 m['salut']").equals("13");


}
