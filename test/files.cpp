#include "Test.hpp"

void Test::test_files() {

	/*
	 * Complex codes
	 */
	header("Primes");

	test_file("test/code/primes.ls", "9592");
	test_file("test/code/gcd.ls", "151");
}
