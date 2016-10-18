#include "Test.hpp"
#include <fstream>
#include <string>

Test::Test() {
	total = 0;
	success_count = 0;
	exeTime = 0;
	obj_deleted = 0;
	obj_created = 0;
}

Test::~Test() {}

int Test::all() {

	clock_t begin = clock();
	exeTime = 0;

	test_general();
	test_booleans();
	test_numbers();
	test_strings();
	test_arrays();
	test_map();
	test_set();
	test_objects();
	test_functions();
	test_classes();
	test_loops();
	test_operators();
	test_references();
	test_operations();
	test_system();
	test_files();

	double elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC;
	int errors = (total - success_count);
	int leaks = (obj_created - obj_deleted);

	std::cout << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Total : " << total << ", success : " << success_count << ", errors : " << errors << std::endl;
	std::cout << "Total time : " << elapsed_secs * 1000 << " ms, execution time : " << (exeTime / CLOCKS_PER_SEC) * 1000 << " ms" << std::endl;
	std::cout << "Objects destroyed : " << obj_deleted << " / " << obj_created << " (" << leaks << " leaked)" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	int result = abs(errors) + abs(leaks);
	if (result == 0) {
		std::cout << "GOOD :)" << std::endl;
	} else {
		std::cout << "BAD : " << result << " error(s) :(" << std::endl;
	}
	std::cout << "------------------------------------------------" << std::endl;

	return result;
}

Test::Input Test::_code(const std::string& code) {
	return Test::Input(this, code, code);
}

Test::Input Test::file(const std::string& file_name) {
	std::ifstream ifs(file_name);
	std::string code = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();
	return Test::Input(this, file_name, code);
}

ls::VM::Result Test::Input::run() {
	auto result = test->vm.execute(_code, "{}");
	test->obj_created += result.objects_created;
	test->obj_deleted += result.objects_deleted;
	test->total++;

	if (result.semantical_errors.size()) {
		for (const auto& error : result.semantical_errors) {
			std::cout << "Semantic error: " << error.message() << std::endl;
		}
	}
	#if DEBUG > 0
		std::cout << "pgrm() " << result.program << std::endl;
	#endif
	return result;
}

#define GREEN "\033[0;32m"
#define RED "\033[1;31m"
#define END_COLOR "\033[0m"

void Test::Input::pass(std::string expected) {
	std::cout << GREEN << "OK   " << END_COLOR << ": " << name() <<  "  ===>  " << expected << std::endl;
	test->success_count++;
}

void Test::Input::fail(std::string expected, std::string actual) {
	std::cout << RED << "FAIL " << END_COLOR << ": " << name() << "  =/=>  " << expected
		<< "  got  " << actual << std::endl;
}

void Test::Input::_equals(std::string&& expected) {

	auto result = run();

	if (result.value == expected) {
		pass(expected);
	} else {
		fail(expected, result.value);
	}
}

template void Test::Input::almost(long expected, long delta);
template void Test::Input::almost(double expected, double delta);

template <typename T>
void Test::Input::almost(T expected, T delta) {

	auto result = run();

	T res_num;
	std::stringstream ss(result.value);
	ss >> res_num;

	if (std::abs(res_num - expected) <= delta) {
		pass(result.value + " (perfect: " + std::to_string(expected) + ")");
	} else {
		fail(std::to_string(expected), result.value);
	}
}

template <typename T>
void Test::Input::between(T a, T b) {

}

void Test::Input::semantic_error(ls::SemanticError::Type expected_type, std::string token) {

	auto result = run();

	std::string expected_message = ls::SemanticError::build_message(expected_type, token);

	if (result.semantical_errors.size()) {
		ls::SemanticError e = result.semantical_errors[0];
		if (expected_type != e.type or token != e.content) {
			fail(expected_message, e.message());
		} else {
			pass(e.message());
		}
	} else {
		fail(expected_message, "(no exception)");
	}
}

void Test::Input::lexical_error(ls::LexicalError::Type expected_type) {

	auto result = run();

	std::string expected_message = ls::LexicalError::build_message(expected_type);

	if (result.lexical_errors.size()) {
		ls::LexicalError e = result.lexical_errors[0];
		if (expected_type != e.type) {
			fail(expected_message, e.message());
		} else {
			pass(e.message());
		}
	} else {
		fail(expected_message, "(no exception)");
	}
}

void Test::Input::operations(int expected) {

	auto result = run();

	if (result.operations != expected) {
		fail(std::to_string(expected), std::to_string(result.operations));
	} else {
		pass(std::to_string(result.operations));
	}
}
Test::Input& Test::Input::timeout(int) {
	return *this;
}

void Test::header(std::string text) {
	std::cout << "----------------" << std::endl;
	std::cout << text << std::endl;
	std::cout << "----------------" << std::endl;
}
