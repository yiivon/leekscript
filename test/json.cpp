#include "Test.hpp"

void Test::test_json() {

	header("Json");

	section("Json.encode()");
	code("Json.encode()").semantic_error(ls::SemanticError::Type::STATIC_METHOD_NOT_FOUND, "Json::encode()");
	// integer
	code("Json.encode(0)").equals("'0'");
	code("Json.encode(12)").equals("'12'");
	code("Json.encode(-589)").equals("'-589'");
	// real
	code("Json.encode(54.123)").equals("'54.123'");
	code("Json.encode(-65.89)").equals("'-65.89'");
	// long
	code("Json.encode(1l)").equals("'1'");
	code("Json.encode(1234567890987)").equals("'1234567890987'");
	// boolean
	code("Json.encode(true)").equals("'true'");
	code("Json.encode(false)").equals("'false'");
	code("Json.encode(12 > 5)").equals("'true'");
	// string
	code("Json.encode('')").equals("'\"\"'");
	code("Json.encode('hello')").equals("'\"hello\"'");
	// array
	code("Json.encode([])").equals("'[]'");
	code("Json.encode([1, 2, 3])").equals("'[1,2,3]'");
	// function : not transformable into JSON
	code("Json.encode(x -> x)").equals("''");
	code("Json.encode([1, x -> x, 3])").equals("'[1,3]'");

	section("Value.json()");
	// integer
	code("0.json()").equals("'0'");
	code("12.json()").equals("'12'");
	code("(-589).json()").equals("'-589'");
	// real
	code("54.123.json()").equals("'54.123'");
	code("(-65.89).json()").equals("'-65.89'");
	// long
	code("(1l).json()").equals("'1'");
	code("1234567890987.json()").equals("'1234567890987'");
	// boolean
	code("true.json()").equals("'true'");
	code("false.json()").equals("'false'");
	code("(12 > 5).json()").equals("'true'");
	// string
	code("''.json()").equals("'\"\"'");
	code("'hello'.json()").equals("'\"hello\"'");
	// array
	code("[].json()").equals("'[]'");
	code("[1, 2, 3].json()").equals("'[1,2,3]'");

	section("Json.decode()");
	code("Json.decode('')").equals("null");
	code("Json.decode('null')").equals("null");
	code("Json.decode('true')").equals("true");
	code("Json.decode('false')").equals("false");

	code("Json.decode('12')").equals("12");
	code("Json.decode('-589')").equals("-589");
	code("Json.decode('54.123')").equals("54.123");
	code("Json.decode('-65.89')").equals("-65.89");
	code("Json.decode('1234567890987')").equals("1234567890987");

	code("Json.decode('\"\"')").equals("''");
	code("Json.decode('\"hello\"')").equals("'hello'");

	code("Json.decode('[]')").equals("[]");
	code("Json.decode('[1,2,3]')").equals("[1, 2, 3]");
	code("Json.decode('[1.6,2.1,3.77]')").equals("[1.6, 2.1, 3.77]");
	code("Json.decode('[\"a\",\"b\",\"c\"]')").equals("['a', 'b', 'c']");
	code("Json.decode('[[],[[],[]],[]]')").equals("[[], [[], []], []]");

}
