#include "Test.hpp"

void Test::test_intervals() {

	header("Intervals");
	code("[1..10]").equals("[1..10]");
	code("[1..Number.sqrt(1989)]").equals("[1..44]");
	code("[5..5]").equals("[5]");
	code("[-100..0]").equals("[-100..0]");
	code("[0..44.523]").equals("[0..44]");
	code("[0l..544l]").equals("[0..544]");
	code("[45.73..248l]").equals("[45..248]");
	// TODO intervals with mpz
	//code("[15m..50m]").equals("[15..50]");

	section("Interval.operator in");
	code("145 in [1..1000]").equals("true");
	code("1 in [1..1000]").equals("true");
	code("1000 in [1..1000]").equals("true");
	code("0 in [1..1000]").equals("false");
	code("1001 in [1..1000]").equals("false");
	code("'a' in [[1..1000], ''][0]").equals("false");
	code("500 in [[1..1000], ''][0]").equals("true");
	code("1500 in [[1..1000], ''][0]").equals("false");

	section("Interval.operator []");
	code("[0..1000][500]").equals("500");
	code("[1..1000][500]").equals("501");
	code("[1000..2000][12]").equals("1012");
	code("[-100..0][5]").equals("-95");
	code("['', [10..20]][1][5]").equals("15");
	code("['', [10..20]][1][50]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10]['hello']").semantic_error( ls::SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, {"'hello'", "[1..10]", ls::Type::STRING_TMP.to_string()});
	code("let i = ['', [10..20]][1] i['hello']").exception(ls::VM::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("[1..10][50]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][-10]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);

	section("Interval.operator [:]");
	code("[5..5][0:0]").equals("[5]");
	code("[1..10][3:5]").equals("[4..6]");
	code("[1..10000000][500000:600000]").equals("[500001..600001]");
	code("[1..10][100:200]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][-1:5]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][20:5]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][5:20]").exception(ls::VM::Exception::ARRAY_OUT_OF_BOUNDS);

	section("|Interval|");
	code("|[0..1]|").equals("2");
	code("|[1..100]|").equals("100");

	section("Iteration");
	code("for k, i in [1..0] { System.print(k + ' => ' + i) }").output("");
	code("for k, i in [1..1] { System.print(k + ' => ' + i) }").output("0 => 1\n");
	code("for k, i in [5..7] { System.print(k + ' => ' + i) }").output("0 => 5\n1 => 6\n2 => 7\n");
	code("var s = 0 for i in [100..110] { s += i } s").equals("1155");
	code("var s = 0 for k, i in [100..110] { s += k * i } s").equals("5885");

	/*
	 * Methods
	 */
	section("Interval.filter()");
	code("[1..6].filter(x -> x > 3)").equals("[4, 5, 6]");
	code("[1..6].filter(x -> x < 3)").equals("[1, 2]");
	code("[1..100].filter(x -> x > 50 and x < 53)").equals("[51, 52]");
	code("[1..Number.sqrt(1989)].filter(x -> 1989 % x == 0)").equals("[1, 3, 9, 13, 17, 39]");
	code("[1..1989.sqrt()].filter(x -> !(1989 % x)).max()").equals("39");
	code("let divisors = n -> [1..n.sqrt()].filter(x -> !(n % x)) divisors(1989)").equals("[1, 3, 9, 13, 17, 39]");

	section("Interval clone()");
	code("let i = [1..10] [i]").equals("[[1..10]]");
}
