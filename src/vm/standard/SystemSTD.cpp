#include "SystemSTD.hpp"

#include <chrono>
#include "../LSValue.hpp"
#include "../../constants.h"
#include "../VM.hpp"

namespace ls {

Compiler::value System_operations(Compiler& c);
Compiler::value System_version(Compiler& c);
long System_time();
long System_millitime();
long System_microtime();
long System_nanotime();
void System_print(LSValue* v);
void System_print_int(int v);
void System_print_mpz(__mpz_struct v);
void System_print_mpz_tmp(__mpz_struct v);
void System_print_long(long v);
void System_print_bool(bool v);
void System_print_float(double v);

SystemSTD::SystemSTD() : Module("System") {

	static_field("version", Type::integer(), System_version);
	static_field("operations", Type::integer(), System_operations);
	static_field("time", Type::long_(), (void*) &System_time);
	static_field("milliTime", Type::long_(), (void*) &System_millitime);
	static_field("microTime", Type::long_(), (void*) &System_microtime);
	static_field("nanoTime", Type::long_(), (void*) &System_nanotime);

	method("print", Method::Static, {
		{{}, {Type::const_any()}, (void*) &System_print, Method::NATIVE},
		{{}, {Type::mpz()}, (void*) &System_print_mpz, Method::NATIVE},
		{{}, {Type::tmp_mpz()}, (void*) &System_print_mpz_tmp, Method::NATIVE},
		{{}, {Type::const_long()}, (void*) &System_print_long, Method::NATIVE},
		{{}, {Type::const_real()}, (void*) &System_print_float, Method::NATIVE},
		{{}, {Type::const_integer()}, (void*) &System_print_int, Method::NATIVE},
		{{}, {Type::const_boolean()}, (void*) &System_print_bool, Method::NATIVE},
	});

	method("throw", {
		{{}, {Type::integer(), Type::i8().pointer(), Type::long_()}, (void*) &SystemSTD::throw1, Method::NATIVE},
		{{}, {Type::long_(), Type::long_(), Type::i8().pointer()}, (void*) &SystemSTD::throw2, Method::NATIVE},
	});
}

long System_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long System_millitime() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long System_microtime() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long System_nanotime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

Compiler::value System_operations(Compiler& c) {
	auto ops_ptr = c.new_pointer(&c.vm->operations, Type::integer().pointer());
	return c.insn_load(ops_ptr);
}

Compiler::value System_version(Compiler& c) {
	return c.new_integer(LEEKSCRIPT_VERSION);
}

void System_print(LSValue* value) {
	value->print(VM::current()->output->stream());
	VM::current()->output->end();
	LSValue::delete_temporary(value);
}

void System_print_int(int v) {
	VM::current()->output->stream() << v;
	VM::current()->output->end();
}

void System_print_mpz(__mpz_struct v) {
	char buff[1000];
	mpz_get_str(buff, 10, &v);
	VM::current()->output->stream() << buff;
	VM::current()->output->end();
}
void System_print_mpz_tmp(__mpz_struct v) {
	char buff[1000];
	mpz_get_str(buff, 10, &v);
	VM::current()->output->stream() << buff;
	VM::current()->output->end();
	mpz_clear(&v);
	VM::current()->mpz_deleted++;
}

void System_print_long(long v) {
	VM::current()->output->stream() << v;
	VM::current()->output->end();
}

void System_print_bool(bool v) {
	VM::current()->output->stream() << std::boolalpha << v;
	VM::current()->output->end();
}

void System_print_float(double v) {
	VM::current()->output->stream() << v;
	VM::current()->output->end();
}

void SystemSTD::throw1(int type, char* function, size_t line) {
	// std::cout << "SystemSTD::throw " << type << " " << function << " " << line << std::endl;
	auto ex = vm::ExceptionObj((vm::Exception) type);
	ex.frames.push_back({function, line});
	throw ex;
}

void fake_ex_destru_fun(void*) {}
void SystemSTD::throw2(void** ex, char* function, size_t line) {
	auto exception = (vm::ExceptionObj*) (ex + 4);
	exception->frames.push_back({function, line});
	__cxa_throw(exception, (void*) &typeid(vm::ExceptionObj), &fake_ex_destru_fun);
}

}
