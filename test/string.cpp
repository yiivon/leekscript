#include "Test.hpp"

void Test::test_strings() {

	header("Strings");

	// General
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
	success("'salut' * (1 + 2)", "'salutsalutsalut'");
	success("('salut' * 1) + 2", "'salut2'");

	// Unicode
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

	// String standard library
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
}
