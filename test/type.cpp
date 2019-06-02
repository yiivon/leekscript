#include "Test.hpp"
#include "../src/colors.h"
#include "../src/type/Array_type.hpp"

void Test::test_types() {

	auto p1 = ls::Type::generate_new_placeholder_type();
	auto p2 = ls::Type::generate_new_placeholder_type();
	auto p3 = ls::Type::generate_new_placeholder_type();

	header("Types");
	section("JSON name");
	assert(ls::Type::long_->getJsonName() == "number");
	assert(ls::Type::map()->getJsonName() == "map");
	assert(ls::Type::interval->getJsonName() == "interval");
	assert(ls::Type::object->getJsonName() == "object");
	assert(ls::Type::clazz()->getJsonName() == "class");
	assert(ls::Type::set()->getJsonName() == "set");
	assert(ls::Type::null->getJsonName() == "null");
	assert(ls::Type::fun()->getJsonName() == "function");

	section("print");
	auto assert_print = [&](const ls::Type* type, const std::string& result) {
		std::ostringstream oss;	oss << type;
		std::cout << oss.str() << std::endl;
		assert(oss.str() == result);
	};
	assert_print(ls::Type::void_, C_GREY "void" END_COLOR);
	assert_print(ls::Type::any, BLUE_BOLD "any" END_COLOR);
	assert_print(ls::Type::null, BLUE_BOLD "null" END_COLOR);
	assert_print(ls::Type::boolean, BLUE_BOLD "bool" END_COLOR);
	assert_print(ls::Type::integer, BLUE_BOLD "int" END_COLOR);
	assert_print(ls::Type::real, BLUE_BOLD "real" END_COLOR);
	assert_print(ls::Type::long_, BLUE_BOLD "long" END_COLOR);
	assert_print(ls::Type::mpz, BLUE_BOLD "mpz" END_COLOR);
	assert_print(ls::Type::array(ls::Type::integer), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">");
	assert_print(ls::Type::array(ls::Type::real), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "real" END_COLOR ">");
	assert_print(ls::Type::array(ls::Type::any), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "any" END_COLOR ">");
	assert_print(ls::Type::object, BLUE_BOLD "object" END_COLOR);
	//assert_print(ls::Type::array(ls::Type::integer).iterator(), BLUE_BOLD "iterator" END_COLOR "<" BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">>");
	assert_print(ls::Type::compound({ls::Type::integer, ls::Type::string}), BLUE_BOLD "int" END_COLOR " | " BLUE_BOLD "string" END_COLOR);

	section("operator ==");
	assert(ls::Type::void_ == ls::Type::void_);
	assert(ls::Type::any == ls::Type::any);
	assert(ls::Type::integer == ls::Type::integer);
	assert(ls::Type::array(ls::Type::integer) == ls::Type::array(ls::Type::integer));
	assert(ls::Type::array(ls::Type::real) == ls::Type::array(ls::Type::real));
	assert(ls::Type::map(ls::Type::any, ls::Type::any) == ls::Type::map(ls::Type::any, ls::Type::any));
	assert(ls::Type::compound({ls::Type::integer, ls::Type::string}) != ls::Type::integer);
	assert(ls::Type::compound({ls::Type::integer, ls::Type::string}) == ls::Type::compound({ls::Type::integer, ls::Type::string}));
	assert(ls::Type::fun(ls::Type::null, {})->return_type() == ls::Type::null);
	assert(ls::Type::mpz->pointer() == ls::Type::mpz_ptr);
	assert(ls::Type::mpz_ptr->pointed() == ls::Type::mpz);
	assert(ls::Type::mpz->pointer()->add_temporary() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::mpz->add_temporary()->pointer() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::mpz_ptr->add_temporary() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::tmp_mpz->pointer() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::tmp_mpz_ptr->pointed() == ls::Type::mpz);
	assert(ls::Type::tmp_mpz->not_temporary() == ls::Type::mpz);
	assert(ls::Type::string->add_temporary() == ls::Type::tmp_string);
	assert(ls::Type::tmp_string->not_temporary() == ls::Type::string);
	assert(ls::Type::mpz->add_constant()->add_temporary() == ls::Type::tmp_mpz);

	section("is_array");
	assert(ls::Type::void_->is_array() == false);
	assert(ls::Type::any->is_array() == false);
	assert(ls::Type::integer->is_array() == false);
	assert(ls::Type::array()->is_array() == true);
	assert(ls::Type::array(ls::Type::any)->is_array() == true);
	assert(ls::Type::array(ls::Type::integer)->is_array() == true);
	assert(ls::Type::array(ls::Type::real)->is_array() == true);
	assert(ls::Type::compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::integer) })->is_array() == true);
	// assert((new ls::Type({ std::make_shared<ls::Array_type>(ls::Type::integer), ls::Type::raw_integer }))->is_array() == false);

	section("castable");
	assert(ls::Type::real->castable(ls::Type::integer));
	assert(ls::Type::array(ls::Type::integer)->castable(ls::Type::array(ls::Type::integer)));
	assert(ls::Type::array(ls::Type::integer)->castable(ls::Type::array()));
	assert(ls::Type::array(ls::Type::real)->castable(ls::Type::array()));
	assert(ls::Type::map(ls::Type::any, ls::Type::any)->castable(ls::Type::map()));
	assert(ls::Type::map(ls::Type::integer, ls::Type::any)->castable(ls::Type::map()));
	assert(ls::Type::map(ls::Type::real, ls::Type::any)->castable(ls::Type::map()));
	assert(ls::Type::any->castable(p1));
	assert(ls::Type::number->castable(ls::Type::boolean));
	assert(ls::Type::boolean->castable(ls::Type::number));
	assert(ls::Type::array(ls::Type::real)->castable(ls::Type::any));
	assert(ls::Type::array(ls::Type::real)->castable(ls::Type::const_any));

	section("Distance");
	assert(ls::Type::number->distance(ls::Type::any) == 1);
	assert(ls::Type::mpz_ptr->distance(ls::Type::any) == 2);

	section("is_number");
	assert(ls::Type::number->is_number());
	assert(ls::Type::long_->is_number());
	assert(ls::Type::mpz->is_number());
	assert(ls::Type::integer->is_number());
	assert(ls::Type::real->is_number());
	assert(ls::Type::boolean->is_number());
	assert(not ls::Type::string->is_number());
	assert(not ls::Type::any->is_number());
	assert(not ls::Type::array()->is_number());
	assert(not ls::Type::map()->is_number());
	assert(not ls::Type::void_->is_number());

	section("is_callable");
	assert(ls::Type::any->is_callable());
	assert(ls::Type::fun()->is_callable());
	assert(ls::Type::clazz()->is_callable());
	assert(not ls::Type::integer->is_callable());
	assert(not ls::Type::array()->is_callable());

	section("operator *");
	test("void * void", ls::Type::void_->operator * (ls::Type::void_), ls::Type::void_);
	test("any * any", ls::Type::any->operator * (ls::Type::any), ls::Type::any);
	test("void * int", ls::Type::void_->operator * (ls::Type::integer), ls::Type::integer);
	test("any * int", ls::Type::any->operator * (ls::Type::integer), ls::Type::any);
	test("int * real", ls::Type::integer->operator * (ls::Type::real), ls::Type::real);
	test("int * string", ls::Type::integer->operator * (ls::Type::string), ls::Type::any);
	test("int * bool", ls::Type::integer->operator * (ls::Type::boolean), ls::Type::any);
	test("any * fun", ls::Type::any->operator * (ls::Type::fun(ls::Type::void_, {})), ls::Type::any);
	test("array<int> * array<real>", ls::Type::array(ls::Type::integer)->operator * (ls::Type::array(ls::Type::real)), ls::Type::any);

	section("fold");
	assert(ls::Type::void_->fold() == ls::Type::void_);
	assert(ls::Type::null->fold() == ls::Type::null);
	assert(ls::Type::compound({ls::Type::integer})->fold() == ls::Type::integer);
	assert(ls::Type::compound({ls::Type::integer, ls::Type::integer})->fold() == ls::Type::integer);
	assert(ls::Type::compound({ls::Type::integer, ls::Type::real})->fold() == ls::Type::real);
	assert(ls::Type::compound({ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real)})->fold() == ls::Type::any);
	assert(ls::Type::tmp_string->fold() == ls::Type::tmp_string);

	section("LLVM type");
	// assert(ls::Type()->llvm_type() == llvm::Type::getVoidTy(ls::Compiler::context));
	// assert(ls::Type::integer->llvm_type() == llvm::Type::getInt32Ty(ls::Compiler::context));
	// assert(ls::Type::boolean->llvm_type() == llvm::Type::getInt1Ty(ls::Compiler::context));
	// assert(ls::Type::real->llvm_type() == llvm::Type::getDoubleTy(ls::Compiler::context));
	// assert(ls::Type({ls::Type::integer, ls::Type::real})->llvm_type() == llvm::Type::getDoubleTy(ls::Compiler::context));

	section("Placeholder types");
	// assert(p1->is_any());

	section("Template types");
	auto T = ls::Type::template_("T");
	T->implement(ls::Type::real);
	// assert(T->is_real());
	// assert(T->fold()->is_real());

	section("Program type");
	code("").type(ls::Type::void_);
	code("null").type(ls::Type::null);
	code("12").type(ls::Type::integer);
	code("12.5").type(ls::Type::real);
	code("'salut'").type(ls::Type::tmp_string);
	code("[]").type(ls::Type::tmp_array(ls::Type::never));
	code("[1]").type(ls::Type::tmp_array(ls::Type::integer));
	code("[1, 2.5]").type(ls::Type::tmp_array(ls::Type::compound({ls::Type::integer, ls::Type::real})));
	code("['a']").type(ls::Type::tmp_array(ls::Type::string));
	code("[[1]]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::integer)));
	code("[[1, 2.5]]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::compound({ls::Type::integer, ls::Type::real}))));
	code("[['a']]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::string)));
}
