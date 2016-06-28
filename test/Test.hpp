#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../leekscript.h"

class Test {

	ls::VM vm;
	int total;
	int success;
	double exeTime;
	int obj_created;
	int obj_deleted;

public:

	Test();
	virtual ~Test();

	void all();
	void header(std::string);
	void test(std::string code, std::string result);
	void ops(std::string code, int operations);

	void test_general();
	void test_operations();
	void test_operators();
	void test_references();
};

#endif
