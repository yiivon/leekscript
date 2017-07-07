#include "Benchmark.hpp"
#include <vector>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <chrono>
#include <sstream>
#include <functional>
#include <iomanip>
#include "../lib/json.hpp"
using namespace std;

std::string pad(std::string s, int l) {
	l -= s.size();
	while (l-- > 0) s += " ";
	return s;
}

long chronotime(std::function<void()> f) {
	auto start = std::chrono::high_resolution_clock::now();
	f();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

std::string format_ns(long ns, long reference = 0) {
	long micro = ns / 1000;
	long ms = micro / 1000;
	std::ostringstream oss;
	oss << ms << " ms";
	if (reference > 0) {
		oss << " x" << std::setprecision(3) << ((double) ns / reference);
	}
	return oss.str();
}

/*
 * Compile and execute the C++ version of a benchmark case
 */
long cpp(const std::string& code) {
	std::ostringstream compile;
	// .exe to clean binary files more easily
	compile << "g++ -O2 -std=c++17 benchmark/code/" << code << "/" << code << ".cpp -o benchmark/code/" << code << "/" << code << ".exe";
	int r = system(compile.str().c_str());
	(void) r;

	std::ostringstream execute;
	execute << "benchmark/code/" << code << "/" << code << ".exe >> results";
	long time = chronotime([&]() {
		int r = system(execute.str().c_str());
		(void) r;
	});

	std::ostringstream clean;
	clean << "rm benchmark/code/" << code << "/" << code << ".exe";
	r = system(clean.str().c_str());

	return time;
}

/*
 * Compile and execute the Java version of a benchmark case
 */
long java(const std::string& code) {
	std::ostringstream compile;
	// .exe to clean binary files more easily
	compile << "javac benchmark/code/" << code << "/" << code << ".java";
	int r = system(compile.str().c_str());
	(void) r;

	std::ostringstream execute;
	execute << "cd benchmark/code/" << code << " && java " << code << " >> ../../../results";
	long time = chronotime([&]() {
		int r = system(execute.str().c_str());
		(void) r;
	});

	std::ostringstream clean;
	clean << "rm benchmark/code/" << code << "/" << code << ".class";
	r = system(clean.str().c_str());

	return time;
}

/*
 * Execute the leekscript version of a benchmark case
 */
long leekscript(const std::string& code) {
	std::ostringstream execute;
	execute << "build/leekscript -n benchmark/code/" << code << "/" << code << ".leek >> results";
	return chronotime([&]() {
		int r = system(execute.str().c_str());
		(void) r;
	});
}

/*
 * Execute the Python version of a benchmark case
 */
long python(const std::string& code) {
	std::ostringstream execute;
	execute << "python benchmark/code/" << code << "/" << code << ".py >> results";
	return chronotime([&]() {
		int r = system(execute.str().c_str());
		(void) r;
	}) * 100; // Python scripts do 100x less work, to have a faster benchmark
}

void benchmark(const std::string& code) {
	int r = system((string("echo \"") + code + "\" >> results").c_str());
	(void) r;
	std::cout << "│ " << pad(code, 22) << "│ " << std::flush;

	long t = cpp(code);
	long ref = t;
	std::cout << pad(format_ns(t, 0), 15) << "│ " << std::flush;

	t = java(code);
	std::cout << pad(format_ns(t, ref), 15) << "│ " << std::flush;

	t = leekscript(code);
	std::cout << pad(format_ns(t, ref), 15) << "│ " << std::flush;

	t = python(code);
	std::cout << "~" << pad(format_ns(t, ref), 14);

	std::cout << "│" << std::endl;
	r = system("echo \"\" >> results");
}

int main(int argc, char** argv) {

	if (argc > 1 && std::string(argv[1]) == "-o") {
		Benchmark::operators();
		return 0;
	}

	cout << "Starting benchmark..." << endl;
	std::remove("results");

	cout << "┌───────────────────────┬────────────────┬────────────────┬────────────────┬────────────────┐" << endl;
	cout << "│ Test case             │ C++ -O2        │ Java           │ LeekScript     │ Python         │" << endl;
	cout << "├───────────────────────┼────────────────┼────────────────┼────────────────┼────────────────┤" << endl;

	long t = chronotime([&]() {
		benchmark("cubic_permutations");
		benchmark("branch_prediction");
		benchmark("primes_sieve");
		benchmark("factorials");
	});

	cout << "└───────────────────────┴────────────────┴────────────────┴────────────────┴────────────────┘" << endl;

	cout << " Total time: " << format_ns(t, 0) << endl;
	cout << endl;
}

string read_file(string file) {
	ifstream ifs(file.data());
	string content = string((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
	ifs.close();
	return content;
}

long run_operator(const std::string& op, bool empty = false) {
	std::ostringstream execute;
	std::string code = std::string("var s = 0.5 for var i = 0; i < 200000; ++i {");
	for (int i = 1; i < 1000; ++i) {
		if (empty) {
			code += std::string("s += ") + std::to_string(i) + std::string(" ");
		} else {
			code += std::string("s += i ") + op + std::string(" ") + std::to_string(i) + std::string(" ");
		}
	}
	code += std::string("}");
	// std::cout << "code: " << code << std::endl;
	int r = system((std::string("build/leekscript -t -j -n \"") + code + std::string("\" > result")).c_str());
	(void) r;
	auto result = Json::parse(read_file("result"));
	return result["time"];
}

void test_operator() {

}

void Benchmark::operators() {
	std::cout << "Starting benchmark..." << std::endl;
	auto b = run_operator("", true);
	std::cout << "base time: " << (b / 1000000) << " ms" << std::endl;

	for (auto& o : {"+", "-", "*", "/", "\\", "^", "&", "|", "%", "%%", "**", "&&"}) {
		auto t = run_operator(o);
		std::cout << "operator " << o << " : " << (t / 1000000) << " ms" << std::endl;
	}
}

void Benchmark::arrays() {

	auto exe_start = chrono::high_resolution_clock::now();

	vector<int> a;
	int n = 1000000;

	for (int i = 0; i < n; ++i) {
		a.push_back(i);
	}
	for (int j = 0; j < n; ++j) {
		a[j] = n - j;
	}

	auto exe_end = chrono::high_resolution_clock::now();
	long exe_time_ns = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();

	cout << (((double) exe_time_ns / 1000) / 1000) << endl;
}

void Benchmark::primes() {

	auto exe_start = chrono::high_resolution_clock::now();


	auto exe_end = chrono::high_resolution_clock::now();
	long exe_time_ns = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();
	double exe_time_ms = (((double) exe_time_ns / 1000) / 1000);
	cout << exe_time_ms << endl;
}
