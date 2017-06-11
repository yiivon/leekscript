#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");
	section("General");
	file("test/code/primes.leek").equals("78498");
	file("test/code/primes_gmp.leek").equals("9591");
	file("test/code/gcd.leek").equals("151");
	file("test/code/strings.leek").almost(52.0, 8.0);
	// TODO
	// file("test/code/reachable_cells.leek").equals("383");
	file("test/code/break_and_continue.leek").equals("2504");
	// TODO
	// file("test/code/french.leek").equals("'cent-soixante-huit millions quatre-cent-quatre-vingt-neuf-mille-neuf-cent-quatre-vingt-dix-neuf'");
	file("test/code/french.min.leek").equals("'neuf-cent-quatre-vingt-sept milliards six-cent-cinquante-quatre millions trois-cent-vingt-et-un-mille-douze'");
	file("test/code/quine.leek").quine();
	// TODO pushAll doesn't work
	// file_v1("test/code/quine_zwik.leek").quine();
	file("test/code/dynamic_operators").works();
	file("test/code/euler1.leek").equals("2333316668");
	file("test/code/text_analysis.leek").equals("[3, 47, 338]");
	// TODO crashing
	// file("test/code/recursive.leek").equals("");
	// file("test/code/references.leek").equals("");
	file("test/code/divisors.leek").equals("[1, 3, 9, 13, 17, 39]");
	file("test/code/two_functions.leek").equals("[{p: 2, v: 5}, [{p: 3, v: 6}]]");
	file("test/code/product_n.leek").equals("5040");
	// TODO leaks
	// file("test/code/product_n_return.leek").equals("5040");
	file("test/code/product_n_arrays.leek").equals("[5040]");
	file("test/code/fold_left.leek").equals("[{w: 1}, {w: 3}, {w: 4}, {w: 2}, {w: 7}, {w: 5}, {w: 8}, {w: 9}, {w: 6}]");
	file("test/code/fold_left_2.leek").equals("{p: 6, v: {p: 9, v: {p: 8, v: {p: 5, v: {p: 7, v: {p: 2, v: {p: 4, v: {p: 3, v: {p: 1, v: []}}}}}}}}}");
	file("test/code/fold_right.leek").equals("[{w: 6}, {w: 9}, {w: 8}, {w: 5}, {w: 7}, {w: 2}, {w: 4}, {w: 3}, {w: 1}]");
	file("test/code/fold_right_2.leek").equals("{p: {p: {p: {p: {p: {p: {p: {p: {p: [], v: 6}, v: 9}, v: 8}, v: 5}, v: 7}, v: 2}, v: 4}, v: 3}, v: 1}");
	file("test/code/assignments.leek").equals("15");
	file("test/code/global_functions_1.leek").equals("false");
	// TODO : better sort
	// file("test/code/global_functions_2.leek").equals("false");

	section("Project Euler");
	file("test/code/euler/pe001.leek").equals("233168");
	file("test/code/euler/pe002.leek").equals("4613732");
	file("test/code/euler/pe003.leek").equals("6857");
	file("test/code/euler/pe004.leek").equals("906609");
	file("test/code/euler/pe005.leek").equals("232792560");
	file("test/code/euler/pe006.leek").equals("25164150");
	file("test/code/euler/pe007.leek").equals("104743");
	file("test/code/euler/pe008.leek").equals("23514624000");
	file("test/code/euler/pe009.leek").equals("31875000");
	file("test/code/euler/pe010.leek").equals("142913828922");
	file("test/code/euler/pe011.leek").equals("70600674");
	file("test/code/euler/pe012.leek").equals("76576500");
	file("test/code/euler/pe013.leek").equals("5537376230");
	// TODO
	// file("test/code/euler/pe014.leek").equals("837799");
	// TODO
	// file("test/code/euler/pe015.leek").equals("137846528820");
	file("test/code/euler/pe016.leek").equals("1366");
	file("test/code/euler/pe062.leek").equals("127035954683");
	file("test/code/euler/pe063.leek").equals("49");
	file("test/code/euler/pe206.leek").equals("1389019170");
}
