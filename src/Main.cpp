#include <unistd.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <string>
#include "../lib/utf8.h"

#include "../leekscript.h"

#include "../test/Test.hpp"
#include "doc/Documentation.hpp"
#include "../benchmark/Benchmark.hpp"

using namespace std;

bool param_time = false;
bool param_verbose = false;
bool param_exec = false;
bool param_file = false;

int main(int argc, char* argv[]) {

//	Benchmark::arrays();
//	Benchmark::primes();

	if (argc > 1 && string(argv[1]) == "-test") {
		Test test;
		test.tests();
		return 0;
	}

	if (argc > 1 && string(argv[1]) == "-doc") {
		ls::Documentation().generate(cout);
		return 0;
	}

	/*
	 * Arguments
	 */
	if (argc > 1 && argv[1][0] == '-') {
		string args(argv[1]);
		for (char c : args) {
			if (c == 't') param_time = true;
			else if (c == 'v') param_verbose = true;
			else if (c == 'e') param_exec = true;
			else if (c == 'f') param_file = true;
		}
	}

	std::string code;
	ls::VM vm;

	if (param_file) {

		string file("");
		if (argc > 2) file = argv[2];
		if (argc == 2 && argv[1][0] != '-') file = argv[1];

		// Read file
		ifstream ifs(file.data());
		code = string((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
		ifs.close();

		// Execute
		vm.execute(code, "{}", ls::ExecMode::NORMAL);

	} else if (param_exec) {

		string code = argv[2];
		string ctx = argv[3];

		vm.execute(code, ctx, ls::ExecMode::COMMAND_JSON);

	} else {

		cout << "~~~ LeekScript v1.0 ~~~" << endl;
		string ctx = "{}";

		while (!std::cin.eof()) {

			// Get a instruction
			cout << ">> ";
			std::getline(std::cin, code);

			// Execute
			ctx = vm.execute(code, ctx, ls::ExecMode::TOP_LEVEL);
		}
	}
	return 0;
}
