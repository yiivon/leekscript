#include "Test.hpp"

void Test::test_types() {

	header("Types");
	// Print any type nature (should not be printed elsewhere)
	std::cout << ls::Type::get_nature_symbol(ls::Nature::ANY) << std::endl;

	// Print a list of types
	std::vector<ls::Type> types { ls::Type::INTEGER, ls::Type::STRING, ls::Type::REAL_ARRAY };
	std::cout << types << std::endl;

	// Print type names
	assert(ls::Type::LONG.raw_type->getJsonName() == "number");
	assert(ls::Type::MAP.raw_type->getJsonName() == "map");
	assert(ls::Type::INTERVAL.raw_type->getJsonName() == "interval");
	assert(ls::Type::OBJECT.raw_type->getJsonName() == "object");
	assert(ls::Type::CLASS.raw_type->getJsonName() == "class");
	assert(ls::Type::SET.raw_type->getJsonName() == "set");
	assert(ls::Type::ANY_OLD.raw_type->getJsonName() == "any_old");
	assert(ls::Type::VOID.raw_type->getName() == "void");
	assert(ls::Type::ANY.raw_type->getName() == "any");
	assert(ls::Type::FUNCTION.raw_type->getName() == "function");

	// Type::more_specific
	assert(ls::Type::more_specific(ls::Type::PTR_ARRAY, ls::Type::INT_ARRAY));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::INT_INT_MAP));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::REAL_REAL_MAP));

	// New types
	auto null = std::make_shared<ls::Null_type>();
	auto number = std::make_shared<ls::Number_type>();
	auto any = std::make_shared<ls::Any_type>();
	auto list_number = std::make_shared<ls::List_type>(number);
	auto list_any = std::make_shared<ls::List_type>(any);
	auto list_list_any = std::make_shared<ls::List_type>(list_any);

	auto list_any_or_number = std::make_shared<ls::LType>();
	list_any_or_number->add(any);
	list_any_or_number->add(number);
	auto list_list_any_or_number = std::make_shared<ls::List_type>(list_any_or_number);

	auto list_number_or_list_any = std::make_shared<ls::LType>();
	list_number_or_list_any->add(list_number);
	list_number_or_list_any->add(list_list_any);

	assert(any->compatible(number) == true);
	assert(list_any->compatible(list_any) == true);
	assert(list_number->compatible(list_any) == false);
	assert(list_any->compatible(list_number) == true);
	assert(list_any->compatible(list_number_or_list_any) == true);
	assert(list_number->compatible(list_list_any_or_number) == false);
}
