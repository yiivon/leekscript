#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");

	file("test/code/primes.leek").equals("78498");
	file("test/code/gcd.leek").equals("151");
	file("test/code/strings.leek").almost(52.0, 2.0);
	//file("test/code/project_euler_62.leek").equals("127035954683");


	//test_file("test/code/array.ls", "1365750");
}
