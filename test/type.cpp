#include "Test.hpp"
#include "../src/type/RawType.hpp"
#include "../src/colors.h"

void Test::test_types() {

	auto p1 = ls::Type::generate_new_placeholder_type();
	auto p2 = ls::Type::generate_new_placeholder_type();
	auto p3 = ls::Type::generate_new_placeholder_type();

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
	assert_print(ls::Type::ANY, BLUE_BOLD "any" END_COLOR);
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
	// assert_print(ls::Type::INT_ARRAY_ITERATOR, BLUE_BOLD "iterator" END_COLOR "<" BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">>");
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

	section("is_array");
	assert(ls::Type().is_array() == false);
	assert(ls::Type::ANY.is_array() == false);
	assert(ls::Type::INTEGER.is_array() == false);
	assert(ls::Type::ARRAY.is_array() == true);
	assert(ls::Type::PTR_ARRAY.is_array() == true);
	assert(ls::Type::INT_ARRAY.is_array() == true);
	assert(ls::Type::REAL_ARRAY.is_array() == true);
	assert(ls::Type({ new ls::Array_type(ls::Type::INTEGER), new ls::Array_type(ls::Type::INTEGER) }).is_array() == true);
	assert(ls::Type({ new ls::Array_type(ls::Type::INTEGER), ls::RawType::INTEGER }).is_array() == false);

	section("castable");
	assert(ls::Type::REAL.castable(ls::Type::INTEGER));
	assert(ls::Type::INT_ARRAY.castable(ls::Type::INT_ARRAY));
	assert(ls::Type::INT_ARRAY.castable(ls::Type::ARRAY));
	assert(ls::Type::REAL_ARRAY.castable(ls::Type::ARRAY));
	assert(ls::Type::REAL_ARRAY.castable(ls::Type::INT_ARRAY));
	assert(ls::Type::INT_ARRAY.castable(ls::Type::REAL_ARRAY));
	assert(ls::Type::MAP.castable(ls::Type::PTR_PTR_MAP));
	assert(ls::Type::MAP.castable(ls::Type::INT_PTR_MAP));
	assert(ls::Type::MAP.castable(ls::Type::REAL_PTR_MAP));
	// assert(ls::Type::ANY.castable(p1));
	assert(ls::Type::NUMBER.castable(ls::Type::BOOLEAN));
	assert(ls::Type::BOOLEAN.castable(ls::Type::NUMBER));
	assert(ls::Type::REAL_ARRAY.castable(ls::Type::ANY));
	assert(ls::Type::REAL_ARRAY.castable(ls::Type::CONST_ANY));

	section("is_number");
	assert(ls::Type::NUMBER.is_number());
	assert(ls::Type::LONG.is_number());
	assert(ls::Type::MPZ.is_number());
	assert(ls::Type::INTEGER.is_number());
	assert(ls::Type::REAL.is_number());
	assert(ls::Type::BOOLEAN.is_number());
	assert(not ls::Type::STRING.is_number());
	assert(not ls::Type::ANY.is_number());
	assert(not ls::Type::ARRAY.is_number());
	assert(not ls::Type::MAP.is_number());
	assert(not ls::Type().is_number());

	section("operator *");
	assert(ls::Type() * ls::Type() == ls::Type());
	assert(ls::Type::ANY * ls::Type::ANY == ls::Type::ANY);
	assert(ls::Type() * ls::Type::INTEGER == ls::Type::INTEGER);
	assert(ls::Type::ANY * ls::Type::INTEGER == ls::Type::ANY);
	assert(ls::Type::INTEGER * ls::Type::REAL == ls::Type::REAL);
	assert(ls::Type::INTEGER * ls::Type::STRING == ls::Type::ANY);

	section("fold");
	assert(ls::Type().fold() == ls::Type());
	assert(ls::Type({ls::RawType::INTEGER}).fold() == ls::Type::INTEGER);
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::INTEGER}).fold() == ls::Type::INTEGER);
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::REAL}).fold() == ls::Type::REAL);
	assert(ls::Type({ls::Type::array(ls::Type::INTEGER), ls::Type::array(ls::Type::REAL)}).fold() == ls::Type::ANY);

	section("LLVM type");
	assert(ls::Type().llvm_type() == llvm::Type::getVoidTy(ls::LLVMCompiler::context));
	assert(ls::Type::ANY.llvm_type() == ls::Any_type::get_any_type());
	assert(ls::Type::INTEGER.llvm_type() == llvm::Type::getInt32Ty(ls::LLVMCompiler::context));
	assert(ls::Type::BOOLEAN.llvm_type() == llvm::Type::getInt1Ty(ls::LLVMCompiler::context));
	assert(ls::Type::REAL.llvm_type() == llvm::Type::getDoubleTy(ls::LLVMCompiler::context));
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::REAL}).llvm_type() == llvm::Type::getDoubleTy(ls::LLVMCompiler::context));
	assert(ls::Type({ls::RawType::INTEGER, ls::RawType::STRING}).llvm_type() == ls::Any_type::get_any_type());
	assert(p1.llvm_type() == ls::Any_type::get_any_type());

	section("Placeholder types");
	assert(p1.is_any());

	section("Program type");
	code("").type(ls::Type());
	code("null").type(ls::Type::NULLL);
	code("12").type(ls::Type::INTEGER);
	code("12.5").type(ls::Type::REAL);
	code("'salut'").type(ls::Type::STRING);
	code("[]").type(ls::Type::PTR_ARRAY);
	code("[1]").type(ls::Type::array(ls::Type::INTEGER));
	code("[1, 2.5]").type(ls::Type::array({ls::RawType::INTEGER, ls::RawType::REAL}));
	code("['a']").type(ls::Type::array(ls::Type::STRING));
	code("[[1]]").type(ls::Type::array(ls::Type::array(ls::Type::INTEGER)));
	code("[[1, 2.5]]").type(ls::Type::array(ls::Type::array({ls::RawType::INTEGER, ls::RawType::REAL})));
	code("[['a']]").type(ls::Type::array(ls::Type::array(ls::Type::STRING)));
}
