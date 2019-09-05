#include "Test.hpp"
#include "../src/doc/Documentation.hpp"

void Test::test_doc() {
	ls::VM vm;

	header("Documentation");
	ls::Documentation doc;
	std::ostringstream oss;
	doc.generate(&vm, oss);
	std::cout << oss.str().substr(0, 300) << " [...]" << std::endl;

	doc.generate(&vm, oss, "wrong_lang");

	auto doc2 = new ls::Documentation();
	delete doc2;
}
