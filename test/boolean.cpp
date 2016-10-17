#include "Test.hpp"

void Test::test_booleans() {

	header("Booléens");

	code("new Boolean").equals("false");
	code("new Boolean()").equals("false");
	code("true").equals("true");
	code("false").equals("false");
	code("!true").equals("false");
	code("!false").equals("true");
	code("true and true").equals("true");
	code("true and false").equals("false");
	code("false and true").equals("false");
	code("false and false").equals("false");
	code("true or true").equals("true");
	code("true or false").equals("true");
	code("false or true").equals("true");
	code("false or false").equals("false");
	code("true xor true").equals("false");
	code("true xor false").equals("true");
	code("false xor true").equals("true");
	code("false xor false").equals("false");
	code("12 xor 5").equals("false");
	code("12 xor 0").equals("true");
	code("false xor 99").equals("true");
	code("false + 99").equals("99");
	code("true + 99").equals("100");

	code("Boolean.compare(true, false)").equals("1");
	code("Boolean.compare(true, true)").equals("0");
	code("Boolean.compare(false, false)").equals("0");
	code("Boolean.compare(false, true)").equals("-1");
}
