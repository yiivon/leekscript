#ifndef BENCHMARK_HPP_
#define BENCHMARK_HPP_

class Benchmark {
public:
	Benchmark();
	virtual ~Benchmark();

	static void arrays();
	static void primes();
	static void operators();
};

#endif
