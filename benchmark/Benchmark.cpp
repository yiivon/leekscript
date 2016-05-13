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
using namespace std;

//#define

Benchmark::Benchmark() {}
Benchmark::~Benchmark() {}

bool is_prime_fast(int number) {

	for (int k = 1; 36 * k * k - 12 * k < number; ++k) {
		if ((number % (6 * k + 1) == 0) or (number % (6 * k - 1) == 0)) {
			return false;
		}
	}
	return true;
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

	int c = 2;
	for (int j = 5; j < 1000000; j += 6) {
		if (is_prime_fast(j)) c++;
		if (is_prime_fast(j + 2)) c++;
	}
	cout << c << endl;

	auto exe_end = chrono::high_resolution_clock::now();
	long exe_time_ns = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();
	double exe_time_ms = (((double) exe_time_ns / 1000) / 1000);
	cout << exe_time_ms << endl;
}

