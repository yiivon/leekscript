#include "Test.hpp"
#include "../src/type/Ty.hpp"
#include "../src/colors.h"

void Test::test_types() {

	header("Types");
	// Print a list of types
	std::vector<ls::Type> types { ls::Type::INTEGER, ls::Type::STRING, ls::Type::REAL_ARRAY };
	std::cout << types << std::endl;

	// Print type names
	assert(ls::Type::LONG.getJsonName() == "number");
	assert(ls::Type::MAP.getJsonName() == "map");
	assert(ls::Type::INTERVAL.getJsonName() == "interval");
	assert(ls::Type::OBJECT.getJsonName() == "object");
	assert(ls::Type::CLASS.getJsonName() == "class");
	assert(ls::Type::SET.getJsonName() == "set");
	assert(ls::Type::NULLL.getJsonName() == "null");
	assert(ls::Type::FUNCTION.getJsonName() == "function");

	auto assert_print = [&](const ls::Type& type, const std::string& result) {
		std::ostringstream oss;	oss << type;
		std::cout << oss.str() << std::endl;
		assert(oss.str() == result);
	};
	assert_print({}, C_GREY "void" END_COLOR);
	assert_print(ls::Type::INTEGER, BLUE_BOLD "int" END_COLOR);
	assert_print(ls::Type::INT_ARRAY, BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">");

	assert(ls::Type::INT_ARRAY == ls::Type::INT_ARRAY);
	assert(ls::Type::REAL_ARRAY == ls::Type::REAL_ARRAY);
	assert(ls::Type::PTR_PTR_MAP == ls::Type::PTR_PTR_MAP);

	assert(ls::Type::REAL.compatible(ls::Type::INTEGER));
	assert(ls::Type::ARRAY.compatible(ls::Type::INT_ARRAY));
	assert(ls::Type::MAP.compatible(ls::Type::PTR_PTR_MAP));
	assert(ls::Type::MAP.compatible(ls::Type::INT_PTR_MAP));
	assert(ls::Type::MAP.compatible(ls::Type::REAL_PTR_MAP));

	assert(ls::Type::get_compatible_type({}, ls::Type::INTEGER) == ls::Type::INTEGER);
	assert(ls::Type::get_compatible_type(ls::Type::INTEGER, ls::Type::REAL) == ls::Type::REAL);

	assert(ls::Type::more_specific(ls::Type::ANY, ls::Type::INTEGER));
	assert(ls::Type::more_specific(ls::Type::PTR_ARRAY, ls::Type::INT_ARRAY));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::INT_INT_MAP));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::REAL_REAL_MAP));

	// New type
	// code("12").type(ls::Ty::get_int());
	// code("12.5").type(ls::Ty::get_double());
	// code("'salut'").type(ls::Ty::get_string());
	// code("[]").type(ls::Ty::array());
	// code("[1]").type(ls::Ty::array(ls::Ty::get_int()));
	// code("[1, 2.5]").type(ls::Ty::array(ls::Ty::get_int()));
	// code("[[1]]").type(ls::Ty::array(ls::Ty::array(ls::Ty::get_int())));

	// std::cout << ls::Ty::get_int().get_compatible(ls::Ty::get_long()) << std::endl;
	// std::cout << ls::Ty::get_int().get_compatible(ls::Ty::get_integer()) << std::endl;
	// std::cout << ls::Ty::get_int().get_compatible(ls::Ty::array()) << std::endl;
}
