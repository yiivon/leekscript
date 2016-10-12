#include "Test.hpp"

void Test::test_files() {

	/*
	 * Complex codes
	 */
	header("Files");

	test_file("test/code/primes.ls", "78498");
	test_file("test/code/gcd.ls", "151");
	//test_file("test/code/array.ls", "1365750");

	file("test/code/strings.ls").almost(52.0, 2.0);

}
