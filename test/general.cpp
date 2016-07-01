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

	/*
	 * General
	 */
	header("General");
	success("", "null");
	success("null", "null");
	success("12", "12");
	success("true", "true");
	success("false", "false");
	success("'toto'", "'toto'");

	/*
	 * Variables
	 */
	header("Variables");
	success("let a = 2", "2");
	success("let a, b, c = 1, 2, 3", "3");
	success("let a", "null");
	success("let a a = 12 a", "12");
	success("let a = 1 let b = (a = 12)", "12");
	success("let s = 'hello'", "'hello'");
	success("let état = 12", "12");
	success("let 韭 = 'leek'", "'leek'");
	success("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ^ 🐨", "25");

	sem_err("a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");
	sem_err("let a = 2 let a = 5", ls::SemanticException::Type::VARIABLE_ALREADY_DEFINED, "a");

	/*
	 * Booléens
	 */
	header("Booléens");
	success("true", "true");
	success("false", "false");
	success("!true", "false");
	success("!false", "true");
	success("true and true", "true");
	success("true and false", "false");
	success("false and true", "false");
	success("false and false", "false");
	success("true or true", "true");
	success("true or false", "true");
	success("false or true", "true");
	success("false or false", "false");
	success("true xor true", "false");
	success("true xor false", "true");
	success("false xor true", "true");
	success("false xor false", "false");
	success("12 xor 5", "false");
	success("12 xor 0", "true");
	success("false xor 99", "true");

	/*
	 * Numbers
	 */
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
	// success("12344532132423", "12344532132423"); large number

	/*
	 * Opérations
	 */
	header("Operations");
	success("5 * 2 + 3 * 4", "22");
	success("let f = x -> x f(5) + f(7)", "12");
	success("'salut' * (1 + 2)", "'salutsalutsalut'");
	success("('salut' * 1) + 2", "'salut2'");

	/*
	 * Arrays
	 */
	header("Arrays");
	success("[]", "[]");
	success("[1]", "[1]");
	success("[1, 2, 3]", "[1, 2, 3]");
	success("[1.21, -5, 4.55, 12, -6.7]", "[1.21, -5, 4.55, 12, -6.7]");
	success("[true, false, true]", "[true, false, true]");
	success("[23, true, '', {}, 123]", "[23, true, '', {}, 123]");
	success("[1, 2, 3] + [4, 5, 6]", "[1, 2, 3, 4, 5, 6]");
	success("[] + 1", "[1]");
	success("[] + 1 + 2 + 3", "[1, 2, 3]");
	success("[1] + [2] + [3]", "[1, 2, 3]");
	success("[1, 2, 3][1]", "2");
	success("let a = [1, 2, 3] a[0]", "1");
	success("let a = [1, 2, 3] a[0] = 5 a[0]", "5");
	success("let a = [1, 2, 3] a[0] += 5 a[0]", "6");
	success("let v = 12 let a = [v, 2, 3] a[0] += 5 a[0]", "17");
	success("let a = [23, 23, true, '', [], 123] |a|", "6");
	success("let a = [1, 2, 3]; ~a", "[3, 2, 1]");
	success("let a = [1, 2, 3] a[1] = 12 a", "[1, 12, 3]");
	success("[1.2, 321.42, 23.15]", "[1.2, 321.42, 23.15]");
	success("[1, 2, 3, 4, 5][1:3]", "[2, 3, 4]");
	success("2 in [1, 2, 3]", "true");
	success("4 in [1, 2, 3]", "false");
	success("'yo' in ['ya', 'yu', 'yo']", "true");
	success("let a = 2 if (a in [1, 2, 3]) { 'ok' } else { 'no' }", "'ok'");

	// let a = [for let i = 0; i < 100; i++ do i end]
	// a[10:*]
	// a[] = 12 (a += 12)

	/*
	 * Strings
	 */
	header("Strings");
	success("'salut ' + 'ça va ?'", "'salut ça va ?'");
	success("'salut' + 12", "'salut12'");
	success("'salut' + true", "'saluttrue'");
	success("'salut' + null", "'salutnull'");
	success("'salut' * 3", "'salutsalutsalut'");
	success("|'salut'|", "5");
	success("~'bonjour'", "'ruojnob'");
	success("'bonjour'[3]", "'j'");
	success("~('salut' + ' ca va ?')", "'? av ac tulas'");
	success("'bonjour'[2:5]", "'njou'");
	success("'韭'", "'韭'");
	success("'♫☯🐖👽'", "'♫☯🐖👽'");
	success("'a♫b☯c🐖d👽'", "'a♫b☯c🐖d👽'");
	success("var hello = '你好，世界'", "'你好，世界'");
	success("'♫☯🐖👽'[3]", "'👽'");
	success("'韭' + '♫'", "'韭♫'");
	success("'♫👽'.size()", "2");
	success("|'♫👽'|", "2");
	success("'☣🦆🧀𑚉𒒫𑓇𐏊'.size()", "7");
	success("'௵௵a௵௵' / 'a'", "['௵௵', '௵௵']");
	success("'a☂a' / '☂'", "['a', 'a']");
	success("~'∑∬∰∜∷⋙∳⌘⊛'", "'⊛⌘∳⋙∷∜∰∬∑'");
	success("'ↂↂ' × 3", "'ↂↂↂↂↂↂ'");
	success("'ḀḂḈḊḖḞḠḦḮḰḸḾṊṎṖ'[5:9]", "'ḞḠḦḮḰ'");

	/*
	 * Références
	 */
	header("References");
	success("let a = 2 let b = a a = 4 b", "2");
	success("let a = 2 let b = @a a = 5 b", "5");
	success("let a = 2 let b = @a b = 6 a", "6");
	success("let a = 2 let b = @a let c = @b a = 7 c", "7");
	success("let a = 2 let b = @a let c = @b c = 8 a", "8");
	success("let a = 'hello' let b = @a a = 'world' b", "'world'");
	success("let a = true let b = @a b = false b", "false");
	success("let v = 10 let a = [v] a[0]++ v", "10");
	//success("let v = 10 let a = [@v] a[0]++ v", "11");

	/*
	 * Conditions
	 */
	header("Conditions");
	success("if true then 12 else 5 end", "12");
	success("if false then 12 else 5 end", "5");
	success("if (true) { 12 } else { 5 }", "12");
	success("if (false) { 12 } else { 5 }", "5");
	success("let a = if (false) { 12 } else { 5 } a", "5");
	success("let a = if (true) { 'yo' } else { 'hello' } a", "'yo'");
	success("let a = if (true) { 12 } else { 'hello' } a", "12");
	success("let a = if (true) { 'hello' } else { 12 } a", "'hello'");

	/*
	 * Functions / Lambdas
	 */
	header("Functions / Lambdas");
	success("let f = x -> x f(12)", "12");
	success("let f = x -> x ^ 2 f(12)", "144");
	success("let f = x, y -> x + y f(5, 12)", "17");
	success("let f = -> 12 f()", "12");
	success("(x -> x)(12)", "12");
	success("(x, y -> x + y)(12, 5)", "17");
	success("( -> [])()", "[]");
	success("( -> 12)()", "12");

//	success("[-> 12][0]()", "12");
//	success("[-> 12, 'toto'][0]()", "12");
//	success("(x -> x + 12.12)(1.01)", "13.13");
//	success("(x -> x + 12)(1.01)", "13.01");
//	success("[x -> x ^ 2][0](12)", "144");
//	success("[[x -> x ^ 2]][0][0](12)", "144");
//	success("[[[x -> x ^ 2]]][0][0][0](12)", "144");
//	success("[[[[[[[x -> x ^ 2]]]]]]][0][0][0][0][0][0][0](12)", "144");

//	success("let f = x -> x (-> f(12))()", "12");
//	success("let f = x -> x let g = x -> f(x) g(12)", "12");
//	success("let g = x -> x ^ 2 let f = x, y -> g(x + y) f(6, 2)", "64");

	success("(-> -> 12)()()", "12");
	success("let f = -> -> 12 f()()", "12");
	success("let f = x -> -> 'salut' f()()", "'salut'");
	success("let f = x -> [x, x, x] f(44)", "[44, 44, 44]");
//	success("let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(10)", "3628800");
//	success("let a = 10 a ~ x -> x ^ 2", "100");
	success("let f = function(x) { let r = x ** 2 return r + 1 } f(10)", "101");
	success("1; 2", "2");
	success("return 1; 2", "1");
//	success("let f = function(x) { if (x < 10) {return true} return 12 } [f(5), f(20)]", "[true, 12]");

	/*
	 * Closures
	 */
	header("Closures");
//	success("let a = 5 let f = -> a f()", "5");
//	success("let f = x -> y -> x + y let g = f(5) g(12)", "17");
//	success("let f = x -> y -> x + y f(5)(12)", "17");
	/*
	 * While loops
	 */
	header("While loops");
	success("let i = 0 while (i < 10) { i++ } i", "10");
	success("let i = 0 let s = 0 while (i < 10) { s += i i++ } s", "45");
	success("let i = 0 while (i < 100) { i++ if (i == 50) break } i", "50");
	success("let i = 0 let a = 0 while (i < 10) { i++ if (i < 8) continue a++ } a", "3");

	/*
	 * For loops
	 */
	header("For loops");
	success("for let i = 0; ; i++ {}", "null");
	success("for let i = 0; i < 10; i++ {}", "null");
	success("let s = 0 for let i = 0; i < 5; i++ do s += i end s", "10");
	success("let s = 0 for let i = 0; i < 10; i += 2 do s += i end s", "20");
	success("let i = 0 for i = 0; i < 10; i++ { } i", "10");
	success("let i = 0 for i = 0; i < 10; i++ { if i == 5 { break } } i", "5");
	success("let a = 0 for let i = 0; i < 10; i++ { a++ } a", "10");
	success("let a = 0 for let i = 0; i < 10; i++ { if i < 5 { continue } a++ } a", "5");

	/*
	 * Foreach loops
	 */
	header("Foreach loops");
	//success("let s = 0 for v in [1, 2, 3, 4] { s += v } s", "10");
	success("let s = '' for v in ['salut ', 'ça ', 'va ?'] { s += v } s", "'salut ça va ?'");
	//success("let s = 0 for k : v in [1, 2, 3, 4] { s += k * v } s", "18");

	/*
	 * Array operations
	 */
	header("Array operations");
	success("[1, 2, 3, 4, 5] ~~ x -> x ^ 2", "[1, 4, 9, 16, 25]");
	success("[1, 2, 3, 4, 5] ~~ (x -> x ^ 2)", "[1, 4, 9, 16, 25]");
	success("['yo', 'toto', 'salut'] ~~ x -> x + ' !'", "['yo !', 'toto !', 'salut !']");
	success("[1, 2, 3] ~~ x -> [x]", "[[1], [2], [3]]");
	success("[1, 2, 3] ~~ x -> 'yo'", "['yo', 'yo', 'yo']");
	success("let f = x -> x * 10 [1, 2, 3] ~~ f", "[10, 20, 30]");
//	success("[1.2, 321.42, 23.15] ~~ x -> x * 1.7", "[2.04, 546.414, 39.355]");


	/*
	 * Intervals
	 */
	header("Intervals");
	success("[1..10]", "[1..10]");
	success("145 in [1..1000]", "true");
	success("1 in [1..1000]", "true");
	success("1000 in [1..1000]", "true");
	success("0 in [1..1000]", "false");
	success("1001 in [1..1000]", "false");
	/*
	success("[1..1000][500]", "501");
	success("[1000..2000][12]", "1012");
	*/

	/*
	 * Swap
	 */
	header("Swap");
	success("let a = 2 let b = 5 a <=> b [a, b]", "[5, 2]");
//	success("let a = [1, 2, 3, 4] a[0] <=> a[3] a", "[4, 2, 3, 1]");
	success("let a = 12 let b = 5 let s = a <=> b", "5");


	/*
	 * Classes
	 */
	header("Classes");
	success("null.class", "<class Null>");
	success("true.class", "<class Boolean>");
	success("false.class", "<class Boolean>");
	success("0.class", "<class Number>");
	success("12.class", "<class Number>");
	success("-5.class", "<class Number>");
	success("12.5234.class", "<class Number>");
	success("''.class", "<class String>");
	success("'salut'.class", "<class String>");
	success("[].class", "<class Array>");
	success("[1, 2, 3].class", "<class Array>");
	success("{}.class", "<class Object>");
	success("{a: 12, b: 'yo'}.class", "<class Object>");
	success("(-> 12).class", "<class Function>");
	success("(x, y -> x + y).class", "<class Function>");
	success("12.class.class", "<class Class>");
/*
	success("class A {} let a = new A", "{}");
	success("class A { let b = 2 } let a = new A", "{}");
	success("class A { let b = 2 } let a = new A a.b", "2");
	success("class A { let b = 2 } let a = new A a.class", "<class A>");
	success("class A {} let t = [A] let a = new t[0]", "{}");
	success("class A {} let a = new A let b = new a.class b.class", "<class A>");
*/
	/*
	 * Function operators
	 */
	header("Function operators");
	success("+(1, 2)", "3");
	success("+([1], 2)", "[1, 2]");
	success("+('test', 2)", "'test2'");
//	success("-(9, 2)", "7");
	success("*(5, 8)", "40");
	success("*('test', 2)", "'testtest'");
	success("/(48, 12)", "4");
	success("^(2, 11)", "2048");
	success("%(48, 5)", "3");
	success("let p = + p(1, 2)", "3");
	success("let p = + p('test', 2)", "'test2'");
	//success("let p = -\n p(9, 2)", "7");
	success("let p = * p(5, 8)", "40");
	success("let p = / p(48, 12)", "4");
	success("let p = % p(48, 5)", "3");
	success("let p = ^ p(2, 11)", "2048");
	success("+", "<function>");
	success("+.class", "<class Function>");
	success("let p = +; p.class", "<class Function>");

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

	/*
	 * Number standard library
	 */
	header("String standard library");
	success("String", "<class String>");
	success("String()", "''");
	success("new String", "''");
	success("new String()", "''");
	success("new String('salut')", "'salut'");
	success("String()", "''");
	success("String('yo')", "'yo'");
	success("String.size('salut')", "5");
	success("String.toUpper('salut')", "'SALUT'");
	success("String.length('salut')", "5");
	success("String.reverse('salut')", "'tulas'");
	success("String.replace('bonjour à tous', 'o', '_')", "'b_nj_ur à t_us'");
	success("String.map('salut', x -> '(' + x + ')')", "'(s)(a)(l)(u)(t)'");
	success("'salut'.map(char -> char + '.')", "'s.a.l.u.t.'");
	success("'♫☯🐖👽韭'.map(u -> u + ' ')", "'♫ ☯ 🐖 👽 韭 '");
	success("String.split('bonjour ça va', ' ')", "['bonjour', 'ça', 'va']");
	success("String.split('bonjour_*_ça_*_va', '_*_')", "['bonjour', 'ça', 'va']");
	success("String.split('salut', '')", "['s', 'a', 'l', 'u', 't']");
	success("String.startsWith('salut ça va', 'salut')", "true");
	success("String.toArray('salut')", "['s', 'a', 'l', 'u', 't']");
	success("String.charAt('salut', 1)", "'a'");
	success("'salut'.substring(3, 4)", "'ut'");

	/*
	 * Array standard library
	 */
	header("Array standard library");
	success("Array", "<class Array>");
	success("Array()", "[]");
	success("new Array", "[]");
	success("new Array()", "[]");

	success("Array.size([1, 'yo', true])", "3");
	success("Array.size([6, 4, 3, 2, -11])", "5");
	success("[1, 'yo', true].size()", "3");
	success("[1, -2, 3, -12, -6].size()", "5");

	success("Array.average([1, 2, 3, 4, 5, 6])", "3.5");
	success("Array.average([])", "0");
	success("[1, 2, 3, 4, 5, 6].average()", "3.5");
	success("[].average()", "0");
	success("['salut', true, []].average()", "0");
	success("[10, -5.7, 30.89, 66].average()", "25.2975");

	success("Array.sum([10, -5, 30, 66])", "101");
	success("[10, -5, 30, 66].sum()", "101");
	success("Array.sum(['ab', 'cd', 'ef'])", "'abcdef'");
	success("['ab', 'cd', 'ef'].sum()", "'abcdef'");
	success("['abc', true, 12, [1, 2]].sum()", "'abctrue12<array>'");
	success("[10, -5.7, 30.89, 66].sum()", "101.19");

	success("Array.map([1, 2, 3], x -> x ^ 2)", "[1, 4, 9]");
	success("[3, 4, 5].map(x -> x ^ 2)", "[9, 16, 25]");
	success("[321, 213, 121].map(x -> x ^ 2).size()", "3");

	success("Array.map2([1, 'yo ', []], [12, 55, 9], (x, y -> x + y))", "[13, 'yo 55', [9]]");

	success("Array.filter([1, 2, 3, 10, true, 'yo'], x -> x > 2)", "[3, 10, 'yo']");
	success("[3, 4, 5].filter(x -> x > 6)", "[]");

	success("Array.contains([1, 2, 3, 10, 1], 1)", "true");
	success("[3, 4, 5].contains(6)", "false");

	success("Array.isEmpty([])", "true");
	success("[3, 4, 5].isEmpty()", "false");
	//success("let a = 0 Array.iter([1,2,3], x -> a += x) a", "6");
	//success("let a = 2 [1,2,3].iter(x -> a *= x) a", "12");

	success("Array.partition([1, 2, 3, 4, 5], (x -> x < 3))", "[[1, 2], [3, 4, 5]]");
	success("Array.partition([1, 2, 3, 10, true, 'yo'], x -> x > 2)", "[[3, 10, 'yo'], [1, 2, true]]");
	success("[1, 2, 3, 4, 5].partition(x -> x > 3)", "[[4, 5], [1, 2, 3]]");
	success("[1, 2, 3, 4, 5].partition(x -> x == 3)", "[[3], [1, 2, 4, 5]]");
	success("[1, 2, 3, 4, 5, 6].filter(x -> x > 2).partition(x -> x > 4)", "[[5, 6], [3, 4]]");
	// success("[1, 2, 3, 4, 5].partition(x -> 'yolo')", "**error**");

	success("Array.first([1, 2, 3, 10, true, 'yo', null])", "1");
	success("['yo', 3, 4, 5].first()", "'yo'");
	success("Array.last([1, 2, 3, 10, true, 'yo', null])", "null");
	// success("['yo', 3, 4, 5].last()", "5");

	// TODO : the return type of first() must be the element type of the array if it's homogeneous
//	success("[[321, 21], [23, 212], [654, 9876]].first().last()", "21");

	success("Array.foldLeft([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')", "'concat:12310trueyonull'");
	success("Array.foldRight([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')", "16");

//	success("Array.shuffle([1, 2, 3, 10, true, 'yo', null])", "test shuffle ?");

	success("Array.reverse([1, 2, 3, 10, true, 'yo', null])", "[null, 'yo', true, 10, 3, 2, 1]");
	success("[null].reverse()", "[null]");
	success("[].reverse()", "[]");
	success("[1, 2, 3].reverse()", "[3, 2, 1]");
	success("Array.search([1, 2, 3, 10, true, 'yo', null], 10, 0)", "3");

	success("Array.search([1, 2, 3, 4, 5], 5, 0)", "4");
	success("Array.search([1, 2, 3, 10, true, 'yo', null], 3, 3)", "-1");
	success("[1, 2, 3, 10, true, 'yo', null].search('yo', 0)", "5");
	success("Array.search([1, 2, 3, 10, true, 'yo', null], false, 0)", "-1");
	success("[null].search(null, 0)", "0");

	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 5)", "[10, true, 'yo']");
	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 1)", "[]");
	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 0, 100)", "[1, 2, 3, 10, true, 'yo', null]");
	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 1, 1)", "[2]");

	success("[].pop()", "null");
	success("Array.pop(['1', '2'])", "'2'");
	success("['1', '2'].pop()", "'2'");
	success("let a = ['1', '2', '3'] a.pop() a", "['1', '2']");

	success("let a = [1, 2, 3] Array.push(a, 4)", "[1, 2, 3, 4]");
	success("[].push([])", "[[]]");
	success("[0].pushAll([1, 2, 3])", "[0, 1, 2, 3]");
	success("let a = [1, 2] a.push(3) a", "[1, 2, 3]");
//	success("let a = [1, 2] a.push(3.5) a", "[1, 2, 3.5]");
	success("let a = [1.5, -2.9] a.push(3.5) a", "[1.5, -2.9, 3.5]");

	success("Array.concat([], [true, 'yo'])", "[true, 'yo']");

	success("[].join('a')", "''");
	success("['salut', 'ça', 'va'].join(' ')", "'salut ça va'");
	success("[1, null, 'va'].join(' ')", "'1 null va'");

	success("let a = [1, 2, 3] a.clear() a", "[]");
	success("let a = [1, 2, 3] Array.clear(a)", "[]");

	success("let a = [1, 2, 3] a.fill(12, 4) a", "[12, 12, 12, 12]");
	success("let a = [] Array.fill(a, 'test', 2)", "['test', 'test']");

	success("let a = [1, 2, 3] Array.insert(a, 12, 1)", "[1, 12, 2, 3]");
	success("let a = [1, 2, 3] Array.insert(a, 12, 6)", "[1, 2, 3, 0, 0, 0, 12]");

	success("let a = [1, 2, 3] Array.remove(a, 1)", "2");
	success("let a = [1, 2, 3] Array.remove(a, 1) a", "[1, 3]");
//	success("let a = [1, 2, 3] Array.remove(a, 1) a", "[0: 1, 2: 3]");
	success("let a = [1, 2, 3] Array.remove(a, 1)", "2");

//	success("let a = [] Array.removeKey(a, 'key')", "null");
//	success("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key')", "'test'");
//	success("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key') a", "[0: 1, 1: 2, 2: 3]");

	success("let a = [1, 2, 3] a.removeElement(1) a", "[2, 3]");
	success("let a = [1, 2, 3] a.removeElement('key') a", "[1, 2, 3]");

//	success("let m = {}")

	/*
	 * Standard library general
	 */
	header("Standard library general");
	//success("let my_map = [].map; my_map([1, 2, 3], x -> x ^ 2)", "[1, 4, 9]");
//	success("[].map == [].map", "true");
//	success("{}.map == {}.map", "true");
//	success("[].map == {}.map", "false");
//	success("let a = [].map; a == [].map", "true");

	/*
	 * Other stuff
	 */
	header("Other");
	//success("var f = obj -> obj.a [f(12), f({a: 'yo'})]", "[null, 'yo']");

/*
	success("3 ~ x -> x ^ x", "27");
	success("[1, 2, 3] ~ x -> x + 4", "[1, 2, 3, 4]");
	success("[1, 2, 3] ~~ print", "[null, null, null]");
	success("[1, 2, 3] ~~ x -> [x, x]", "[[1, 1], [2, 2], [3, 3]]");
	success("let s = 0 let a = [1, 2, 3, 4] let f = x -> s += x a ~~ f s", "10");
	success("let s = 0 [1, 2, 3, 4, 5].each(x -> s += x)", "15");
	success("[1, 2, 3, 4, 5].fold_left(+,0)", "15");
	//success("let f = x -> x + 10 let g = x -> x ^ 2 [1, 2, 3] ~~ f ~~ g", "[121, 144, 169]");
	//success("[1, 2, 3] ~~ (x -> x * 5) ~~ (x -> x - 1)", "[4, 9, 14]");
*/

	/*
	a ~~ b => (a ~~ b)
	a ~ ~b => a ~ (~b)
	~~a => ~(~a)
	a; ~~b
	*/

	/*
	[1, 2, 3] ~~= (x -> x * 5 + 2)
	[1, 2, 3] ~+= 1   =>   [2, 3, 4]
	[1, 2, 3] ~*= 5
	[1, 2, 3] ~= (x -> x * 5)

	var dist = [...]
	dist ~*= 5
	var newdist = dist ~* 5


	var a
	;[1, 2, 3].map(...)
*/


}


