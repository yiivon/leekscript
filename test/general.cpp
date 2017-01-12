#include <string>
#include <iostream>
#include <sstream>

#include "../src/vm/Context.hpp"
#include "../src/compiler/lexical/LexicalAnalyser.hpp"
#include "../src/compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "../src/compiler/semantic/SemanticAnalyser.hpp"
#include "../src/compiler/semantic/SemanticError.hpp"
#include "Test.hpp"

using namespace std;

void Test::test_general() {

	header("General");
	code("").equals("(void)");
	code(" ").equals("(void)"); // classic space
	code(" ").equals("(void)"); // unbreakable space
	code("	").equals("(void)"); // tab
	code("null").equals("null");
	// TODO syntaxical error
	//code("()").syntaxical_error();
	code("12").equals("12");
	code("true").equals("true");
	code("false").equals("false");
	code("'toto'").equals("'toto'");
	code("[]").equals("[]");
	code("{}").equals("{}");
	code("{a: 12}").equals("{a: 12}");
	code("{;}").equals("(void)");
	code("return 12").equals("12");
	code("return").equals("(void)");
	code("'a' 'b' 'c'").equals("'c'");

	section("Parenthesis");
	code("(5)").equals("5");
	code("(((((5)))))").equals("5");
	code("(5) * (4 + 2) - 7 / (4 - (((5))))").equals("37");

	section("Line breaks");
	code("5 \n + 5").equals("10");
	code("5 \n - 5").equals("-5");

	header("Variables");
	code("let a = 2 a").equals("2");
	code("let a = 1, b = 2, c = 3").equals("(void)");
	code("let a = 1, b = 2, c").equals("(void)");
	code("let a = 1, b, c = 3").equals("(void)");
	code("let a, b, c = 3").equals("(void)");
	code("let a, b, c").equals("(void)");
	code("let a").equals("(void)");
	code("let a a").equals("null");
	//code("let a a = 12 a").equals("12");
	code("var a = 1 let b = (a = 12) b").equals("12");
	code("let s = 'hello'").equals("(void)");
	code("let s = 'hello' s").equals("'hello'");
	code("let état = 12 état").equals("12");
	code("let 韭 = 'leek' 韭").equals("'leek'");
	code("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ** 🐨").equals("25");

	section("Global variables");
	code("global a = 2").equals("(void)");
	code("global a = 2, b = 'a'").equals("(void)");

	code("a").semantic_error(ls::SemanticError::Type::UNDEFINED_VARIABLE, {"a"});
	code("let a = 2 let a = 5").semantic_error(ls::SemanticError::Type::VARIABLE_ALREADY_DEFINED, {"a"});

	code("let a = 12 a").equals("12");
	code("let a = 12 { let a = 5 } a").equals("12");
	code("let a = 12 var b = 0 { let a = 5 b = a } b").equals("5");
	code("{let a = 5} a").semantic_error(ls::SemanticError::Type::UNDEFINED_VARIABLE, {"a"});

	section("Syntaxic errors");
	code("{").syntaxic_error(ls::SyntaxicalError::Type::BLOCK_NOT_CLOSED, {});
	code("2 + ()").syntaxic_error(ls::SyntaxicalError::Type::EXPECTED_VALUE, {")"});
	code("let 2 = 5").syntaxic_error(ls::SyntaxicalError::Type::UNEXPECTED_TOKEN, {"2"});

	section("Syntaxic edge cases");
	code("let toto = 2; toto'salut'").equals("'salut'"); // string just after a ident
	code("let toto = 2; toto\"salut\"").equals("'salut'"); // string just after a ident
	code("12345'salut'").equals("'salut'"); // string just after a number
	code("12345\"salut\"").equals("'salut'"); // string just after a number

	section("?? operator");
	code("'foo' ?? 'bar'").equals("'foo'");
	code("null ?? 'bar'").equals("'bar'");
	code("let a = 'foo' a ?? 'bar'").equals("'foo'");
	code("let a = null a ?? 'bar'").equals("'bar'");
	code("[] ?? [12]").equals("[]");
	code("null ?? [12]").equals("[12]");
	code("{} ?? 'default'").equals("{}");
	code("null ?? 'default'").equals("'default'");
	code("let a = null let b = null a ?? b ?? ':)'").equals("':)'");

	section("Value.string()");
	// integer
	code("0.string()").equals("'0'");
	code("12.string()").equals("'12'");
	code("(-7).string()").equals("'-7'");
	// long
	code("12434324223112123.string()").equals("'12434324223112123'");
	code("(-1243439967898452).string()").equals("'-1243439967898452'");
	// real
	code("12.5.string()").equals("'12.5'");
	code("(-6546.34).string()").equals("'-6546.34'");
	code("Number.pi.string()").equals("'3.1415926536'");
	// boolean
	code("true.string()").equals("'true'");
	code("false.string()").equals("'false'");
	code("(12 > 5).string()").equals("'true'");
	// TODO more types

	section("Const values");
	code("var c = 2 c++").equals("2");
	code("let c = 2 c++").semantic_error(ls::SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, {"c"});
	code("let c = 2; ++c").semantic_error(ls::SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, {"c"});
}
