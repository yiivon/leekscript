#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");

	file("test/code/primes.ls").equals("78498");
	file("test/code/gcd.ls").equals("151");
	file("test/code/strings.ls").almost(52.0, 2.0);

	//test_file("test/code/array.ls", "1365750");

}
