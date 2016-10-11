#include "Test.hpp"

void Test::test_files() {

	/*
	 * Complex codes
	 */
	header("Files");

	test_file("test/code/primes.ls", "78498");
	test_file("test/code/gcd.ls", "151");
	test_file("test/code/array.ls", "1365750");

	//test_file("test/code/strings.ls", "52"); // almost 52 (+- 2)

	// TODO
	/*
	 code("2 + 2").equals("4")
	 code("'hello'()").error(SemanticException::CANNOT_CALL_VALUE, "'hello'")
	 code("'unterminated").error(LexicalError::UNTERMINATED_STRING)
	 code("[1, 2, 3].size()").between(2, 5)
	 file("test/code/strings.ls").almost(52, 2)
	 code("1 + 1").operations(1)

	 file("long_code.ls").timeout(500).equals(32312)

 	 code("while (true) {}").timeout(10).error(Timeout)

	*/

}
