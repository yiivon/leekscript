#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../vm/VM.hpp"

class Test {

	VM vm;
	int total;
	int success;
	double exeTime;

public:

	Test();
	virtual ~Test();

	void tests();
	void header(std::string);

	void test(std::string code, std::string result);
};

#endif
