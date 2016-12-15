#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../src/leekscript.h"
#include "../src/vm/VM.hpp"
#include "../src/compiler/semantic/SemanticError.hpp"
#include "../src/compiler/lexical/LexicalError.hpp"

class Test {
private:
	ls::VM vm;
	int total;
	int success_count;
	double exeTime;
	int obj_created;
	int obj_deleted;
	int gmp_obj_created;
	int gmp_obj_deleted;
	static std::vector<std::string> failed_tests;

public:

	class Input;

	Test();
	virtual ~Test();

	int all();
	void header(std::string);
	void section(std::string);

	Input code(const std::string& _code);
	Input code_v1(const std::string& _code);
	Input file(const std::string& file_name);
	Input file_v1(const std::string& file_name);

	void test_general();
	void test_operations();
	void test_operators();
	void test_references();
	void test_system();
	void test_objects();
	void test_strings();
	void test_numbers();
	void test_booleans();
	void test_arrays();
	void test_map();
	void test_set();
	void test_functions();
	void test_loops();
	void test_classes();
	void test_files();
	void test_doc();
	void test_intervals();
	void test_json();

	class Input {
	public:
		Test* test;
		std::string name;
		std::string code;
		bool file;
		bool v1;
		float compilation_time;
		float execution_time;
		long int operation_limit = ls::VM::DEFAULT_OPERATION_LIMIT;

		Input(Test* test, const std::string& name, const std::string& code,
			bool file = false, bool v1 = false) : test(test), name(name), code(code), file(file), v1(v1) {};
		void works();
		void equals(std::string&& expected);
		template <typename T>
		void almost(T expected, T delta = 1e-10);
		template <typename T>
		void between(T a, T b);
		void semantic_error(ls::SemanticError::Type error, std::vector<std::string> params);
		void lexical_error(ls::LexicalError::Type error);
		void operations(int ops);
		void exception(ls::VM::Exception);
		void output(std::string expected);
		void quine();
		Input& timeout(int ms);
		Input& ops_limit(long int ops);

		ls::VM::Result run(bool display_errors = true);
		void pass(std::string expected);
		void fail(std::string expected, std::string actuel);
	};
};




#endif
