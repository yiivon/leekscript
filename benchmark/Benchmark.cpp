#include "Benchmark.hpp"
#include <vector>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <string>
using namespace std;

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

void primes() {

	clock_t begin = clock();
	int c = 2;
	for (int j = 5; j < 1000000; j += 6) {
		if (is_prime_fast(j)) c++;
		if (is_prime_fast(j + 2)) c++;
	}

	clock_t end = clock();

	cout << c << endl;

	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "primes time : " << elapsed_secs * 1000 << "ms" << endl;
}
