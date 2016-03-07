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

	if (((!(number & 1)) and number != 2) or (number < 2) or (number % 3 == 0 and number != 3)) {
		return false;
	}

	for (int k = 1; 36 * k * k - 12 * k < number; ++k) {
		if ((number % (6 * k + 1) == 0) or (number % (6 * k - 1) == 0)) {
			return false;
		}
	}
	return true;
}


bool is_prime(int n) {

	int root = sqrt(n);

	for (int q = 3; q <= root; q += 2) {
		if ((n % q) == 0) {
			return false;
		}
	}
	return true;
}

void primes() {

	clock_t begin = clock();

	int c = 1;
	vector<int> primes;

	for (int i = 3; i < 1000000; i += 2) {
		if (is_prime_fast(i)) {
			c++;
		}
	}

	clock_t end = clock();

	cout << c << endl;

	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "primes time : " << elapsed_secs * 1000 << "ms" << endl;
}
