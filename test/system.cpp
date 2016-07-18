#include "Test.hpp"

#include <chrono>
#include "../src/compiler/semantic/SemanticException.hpp"

long get_sec_time();
long get_milli_time();
long get_micro_time();
long get_nano_time();

void Test::test_system() {

	header("System");

	success("System.version", "1");

	success("System.operations", "0");
	success("2 + 2 System.operations", "1");

	long sec_time = get_sec_time();
	success_almost("System.time", sec_time, 1L);

	long milli_time = get_milli_time();
	success_almost("System.milliTime", milli_time, 100L);

	long micro_time = get_micro_time();
	success_almost("System.microTime", micro_time, 100000L);

	long nano_time = get_nano_time();
	success_almost("System.nanoTime", nano_time, 100000000L);

	sem_err("let a = System.print(12)", ls::SemanticException::Type::CANT_ASSIGN_VOID, "a");
}

long get_sec_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_milli_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_micro_time() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_nano_time() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}
