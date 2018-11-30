#include "Test.hpp"
#include "../src/type/RawType.hpp"
#include "../src/colors.h"

void Test::test_types() {

	header("Types");
	section("JSON name");
	assert(ls::Type::LONG.getJsonName() == "number");
	assert(ls::Type::MAP.getJsonName() == "map");
	assert(ls::Type::INTERVAL.getJsonName() == "interval");
	assert(ls::Type::OBJECT.getJsonName() == "object");
	assert(ls::Type::CLASS.getJsonName() == "class");
	assert(ls::Type::SET.getJsonName() == "set");
	assert(ls::Type::NULLL.getJsonName() == "null");
	assert(ls::Type::FUNCTION.getJsonName() == "function");

	section("print");
	auto assert_print = [&](const ls::Type& type, const std::string& result) {
		std::ostringstream oss;	oss << type;
		std::cout << oss.str() << std::endl;
		assert(oss.str() == result);
	};
	assert_print({}, C_GREY "void" END_COLOR);
	assert_print(ls::Type::NULLL, BLUE_BOLD "null" END_COLOR);
	assert_print(ls::Type::BOOLEAN, BLUE_BOLD "bool" END_COLOR);
	assert_print(ls::Type::INTEGER, BLUE_BOLD "int" END_COLOR);
	assert_print(ls::Type::REAL, BLUE_BOLD "real" END_COLOR);
	assert_print(ls::Type::LONG, BLUE_BOLD "long" END_COLOR);
	assert_print(ls::Type::MPZ, BLUE_BOLD "mpz" END_COLOR);
	assert_print(ls::Type::INT_ARRAY, BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">");
	assert_print(ls::Type::REAL_ARRAY, BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "real" END_COLOR ">");
	assert_print(ls::Type::PTR_ARRAY, BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "any" END_COLOR ">");
	assert_print(ls::Type::OBJECT, BLUE_BOLD "object" END_COLOR);
	assert_print(ls::Type::INT_ARRAY_ITERATOR, BLUE_BOLD "iterator" END_COLOR "<" BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">>");
	assert_print(ls::Type({ls::RawType::INTEGER, ls::RawType::STRING}), BLUE_BOLD "int" END_COLOR " | " BLUE_BOLD "string" END_COLOR);

	section("operator ==");
	assert(ls::Type() == ls::Type());
	assert(ls::Type::ANY == ls::Type::ANY);
	assert(ls::Type::INTEGER == ls::Type::INTEGER);
	assert(ls::Type::INT_ARRAY == ls::Type::INT_ARRAY);
	assert(ls::Type::REAL_ARRAY == ls::Type::REAL_ARRAY);
	assert(ls::Type::PTR_PTR_MAP == ls::Type::PTR_PTR_MAP);
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::STRING}) != ls::Type::INTEGER);
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::STRING}) == ls::Type({ls::RawType::INTEGER, ls::RawType::STRING}));

	section("compatible");
	assert(ls::Type::REAL.compatible(ls::Type::INTEGER));
	assert(ls::Type::ARRAY.compatible(ls::Type::INT_ARRAY));
	assert(ls::Type::MAP.compatible(ls::Type::PTR_PTR_MAP));
	assert(ls::Type::MAP.compatible(ls::Type::INT_PTR_MAP));
	assert(ls::Type::MAP.compatible(ls::Type::REAL_PTR_MAP));

	section("operator *");
	assert(ls::Type() * ls::Type() == ls::Type());
	assert(ls::Type::ANY * ls::Type::ANY == ls::Type::ANY);
	assert(ls::Type() * ls::Type::INTEGER == ls::Type::INTEGER);
	assert(ls::Type::ANY * ls::Type::INTEGER == ls::Type::ANY);
	assert(ls::Type::INTEGER * ls::Type::REAL == ls::Type::REAL);
	assert(ls::Type::INTEGER * ls::Type::STRING == ls::Type::ANY);

	section("more_specific");
	assert(ls::Type::more_specific(ls::Type::ANY, ls::Type::INTEGER));
	assert(ls::Type::more_specific(ls::Type::PTR_ARRAY, ls::Type::INT_ARRAY));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::INT_INT_MAP));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::REAL_REAL_MAP));

	section("fold");
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::INTEGER}).fold() == ls::Type::INTEGER);
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::REAL}).fold() == ls::Type::REAL);

	section("LLVM type");
	assert(ls::Type::INTEGER.llvm_type() == llvm::Type::getInt32Ty(ls::LLVMCompiler::context));
	assert(ls::Type::REAL.llvm_type() == llvm::Type::getDoubleTy(ls::LLVMCompiler::context));
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::REAL}).llvm_type() == llvm::Type::getInt32Ty(ls::LLVMCompiler::context));

	section("Program type");
	code("").type(ls::Type());
	code("null").type(ls::Type::NULLL);
	code("12").type(ls::Type::INTEGER);
	code("12.5").type(ls::Type::REAL);
	code("'salut'").type(ls::Type::STRING);
	code("[]").type(ls::Type::PTR_ARRAY);
	code("[1]").type(ls::Type::array(ls::Type::INTEGER));
	code("[1, 2.5]").type(ls::Type::array(ls::Type::REAL));
	code("['a']").type(ls::Type::array(ls::Type::STRING));
	code("[[1]]").type(ls::Type::array(ls::Type::array(ls::Type::INTEGER)));
	code("[[1, 2.5]]").type(ls::Type::array(ls::Type::array(ls::Type::REAL)));
	code("[['a']]").type(ls::Type::array(ls::Type::array(ls::Type::STRING)));

	// std::cout << ls::Ty::get_int().get_compatible(ls::Ty::get_long()) << std::endl;
	// std::cout << ls::Ty::get_int().get_compatible(ls::Ty::get_integer()) << std::endl;
	// std::cout << ls::Ty::get_int().get_compatible(ls::Ty::array()) << std::endl;
}
