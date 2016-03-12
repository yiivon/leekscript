#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>

class Test {

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
