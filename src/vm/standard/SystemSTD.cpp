#include "SystemSTD.hpp"

#include "../LSValue.hpp"

namespace ls {

SystemSTD::SystemSTD() : Module("System") {

	static_field("operations", Type::INTEGER, "0");

	static_field("time", Type::LONG, "0");
	static_field("milliTime", Type::LONG, "0");
	static_field("microTime", Type::LONG, "0");
	static_field("nanoTime", Type::LONG, "0");

	static_method("print", {
		{Type::VOID, {Type::INTEGER}, (void*) &System_print_int},
		{Type::VOID, {Type::LONG}, (void*) &System_print_long},
		{Type::VOID, {Type::BOOLEAN}, (void*) &System_print_bool},
		{Type::VOID, {Type::FLOAT}, (void*) &System_print_float},
		{Type::VOID, {Type::POINTER}, (void*) &System_print}
	});
}

void System_print(LSValue* value) {
	value->print(std::cout);
	std::cout << std::endl;
}

void System_print_int(int v) {
	std::cout << v << std::endl;
}

void System_print_long(long v) {
	std::cout << v << std::endl;
}

void System_print_bool(bool v) {
	std::cout << std::boolalpha << v << std::endl;
}

void System_print_float(double v) {
	std::cout << v << std::endl;
}

}
