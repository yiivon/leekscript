#include "Test.hpp"
#include "../src/util/Util.hpp"

void Test::test_utils() {

	Util::is_file_name("Makefile"); // true
	Util::is_file_name("12 + 5"); // false
	Util::is_file_name("salut.leek"); // true
	Util::is_file_name("hello.ls"); // true
	Util::is_file_name("helloworld"); // false
}
