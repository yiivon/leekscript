#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
using namespace std;

class Test {

	int total;
	int success;
	double exeTime;

public:

	Test();
	virtual ~Test();

	void tests();
	void header(string);

	void test(string code, string result);
};

#endif
