#include "Test.hpp"

void Test::test_numbers() {

	header("Numbers");

	success("0", "0");
	success("-1", "-1");
	success("-(-1)", "1");
	success("0 + 5", "5");
	success("5 + 5", "10");
	success("10 - 3", "7");
	success("5 * 5", "25");
	success("15 / 3", "5");
	success("15 / 2", "7.5");
	success("12 ^ 2", "144");
	success("2 ^ 5", "32");
	success("2 < 5", "true");
	success("12 < 5", "false");
	success("5 == 12", "false");
	success("12 == 12", "true");
	success("let a = 2 a++", "2");
	success("let a = 2; ++a", "3");
	success("let a = 2 a--", "2");
	success("let a = 2; --a", "1");
	success("let a = 2 a += 5", "7");
	success("let a = 2 a -= 5", "-3");
	success("let a = 2 a *= 5", "10");
	success("let a = 100 a /= 5", "20");
	success("let a = 56 a %= 17", "5");
	success("let a = 15 a ^= 2", "225");
	success("(33 - 2) / 2", "15.5");
	success("12 < (45 / 4)", "false");
	success("12 == (24 / 2)", "true");
	success("2.5 + 4.7", "7.2");
	success("2.5 × 4.7", "11.75");
	success("π", "3.1415926536");
	success("5 * 2 + 3 * 4", "22");

	// success("12344532132423", "12344532132423"); large number

	/*
	 * Number standard library
	 */
	header("Number standard library");
	success("Number", "<class Number>");
	success("Number()", "0");
	success("new Number", "0");
	success("new Number()", "0");
	success("new Number(12)", "12");
//	success("Number.abs", "<function>");
	success("Number.abs(-12)", "12");
	success("Number.floor(5.9)", "5");
	success("var a = 5 Number.floor(a)", "5");
	success("var a = 5.4 Number.floor(a)", "5");
	success("Number.round(5.7)", "6");
	success("Number.round(5.4)", "5");
	success("Number.ceil(5.1)", "6");
	success("Number.max(5, 12)", "12");
	success("Number.cos(0)", "1");
	success("Number.cos(π)", "-1");
	success("Number.cos(π / 2)", "0");
	success("Number.sin(0)", "0");
	success("Number.sin(π)", "0");
	success("Number.sin(π / 2)", "1");
	success("Number.sin(- π / 2)", "-1");
//	success("12.sqrt()", "");
}
