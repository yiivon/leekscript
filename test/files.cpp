#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");
	file("test/code/primes.leek").equals("78498");
	file("test/code/gcd.leek").equals("151");
	file("test/code/strings.leek").almost(52.0, 2.0);
	file("test/code/project_euler_62.leek").equals("127035954683");
	file("test/code/reachable_cells.leek").equals("383");

	file("test/code/euler/pe001.leek").equals("233168");
	file("test/code/euler/pe002.leek").equals("4613732");
	file("test/code/euler/pe003.leek").equals("6857");
	file("test/code/euler/pe004.leek").equals("906609");
	file("test/code/euler/pe005.leek").equals("232792560");
	file("test/code/euler/pe006.leek").equals("25164150");
	file("test/code/euler/pe062.leek").equals("127035954683");
}
