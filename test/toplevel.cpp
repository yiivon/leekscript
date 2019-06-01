#include "Test.hpp"
#include "../src/vm/Context.hpp"
#include "../src/vm/value/LSString.hpp"

void Test::test_toplevel() {
	header("Top-level");
	ls::Context ctx;

	code("var a = 2").context(&ctx).works();
	test("Type of a", ctx.vars.at("a").type, ls::Type::integer());
	test("Value of a", (int) (long) ctx.vars.at("a").value, 2);
	test("Size of context", ctx.vars.size(), 1ul);
	code("a * 7").context(&ctx).equals("14");
	code("a + 'salut'").context(&ctx).equals("'2salut'");
	code("a").error(ls::Error::UNDEFINED_VARIABLE, {"a"});

	code("var b = 3.14").context(&ctx).works();
	test("Size of context", ctx.vars.size(), 2ul);
	test("Type of b", ctx.vars.at("b").type, ls::Type::real());
	test("Value of b", reinterpret_cast<double&>(ctx.vars.at("b").value), 3.14);

	code("var c = 'salut'").context(&ctx).works();
	test("Size of context", ctx.vars.size(), 3ul);
	test("Type of c", ctx.vars.at("c").type, ls::Type::any());
	test("Value of c", *static_cast<ls::LSString*>(ctx.vars.at("c").value), ls::LSString("salut"));
	code("c + b + a").context(&ctx).equals("'salut3.142'");

	code("c += ' !'").context(&ctx).works();
	test("Size of context", ctx.vars.size(), 3ul);
	test("Type of c", ctx.vars.at("c").type, ls::Type::any());
	test("Value of c", *static_cast<ls::LSString*>(ctx.vars.at("c").value), ls::LSString("salut !"));
}
