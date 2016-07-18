#include <string>
#include <iostream>
#include <sstream>

#include "../src/vm/Context.hpp"
#include "../src/compiler/lexical/LexicalAnalyser.hpp"
#include "../src/compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "../src/compiler/semantic/SemanticAnalyser.hpp"
#include "../src/compiler/semantic/SemanticException.hpp"
#include "Test.hpp"

using namespace std;

void Test::test_general() {

	header("General");
	success("", "null");
	success("null", "null");
	success("()", "null");
	success("12", "12");
	success("true", "true");
	success("false", "false");
	success("'toto'", "'toto'");
	success("[]", "[]");
	success("{}", "{}");
	success("{a: 12}", "{a: 12}");
	success("{;}", "null");

	header("Variables");
	success("let a = 2", "2");
	success("let a, b, c = 1, 2, 3", "3");
	success("let a", "null");
	success("let a a = 12 a", "12");
	success("let a = 1 let b = (a = 12)", "12");
	success("let s = 'hello'", "'hello'");
	success("let état = 12", "12");
	success("let 韭 = 'leek'", "'leek'");
	success("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ** 🐨", "25");

	sem_err("a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");
	sem_err("let a = 2 let a = 5", ls::SemanticException::Type::VARIABLE_ALREADY_DEFINED, "a");

	success("let a = 12 { let a = 5 } a", "12");
	success("let a = 12 let b = 0 { let a = 5 b = a } b", "5");
	sem_err("{let a = 5} a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");
}


