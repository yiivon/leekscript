#include "Test.hpp"

void Test::test_json() {

	header("Json");

	section("Json.encode()");
	code("Json.encode()").semantic_error(ls::SemanticError::Type::STATIC_METHOD_NOT_FOUND, "Json::encode()");
	code("Json.encode(0)").equals("'0'");
	code("Json.encode(12)").equals("'12'");
	code("Json.encode(-589)").equals("'-589'");
	code("Json.encode(54.123)").equals("'54.123'");
	code("Json.encode(-65.89)").equals("'-65.89'");

	code("Json.encode(true)").equals("'true'");
	code("Json.encode(false)").equals("'false'");
	code("Json.encode(12 > 5)").equals("'true'");

	section("Array.json()");
	code("[1, 2, 3].json()").equals("'[1,2,3]'");

}
