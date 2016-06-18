#include "../../compiler/lexical/Operator.hpp"

#include <iostream>
using namespace std;

namespace ls {

/*
 * Binary operators precedence
 * ------------------
 * 0| ~ ~~ ~= ~~=
 * ------------------
 * 1| ^
 * ------------------
 * 2| * / %
 * ------------------
 * 3| + -
 * ------------------
 * 4| < <= > >=
 * ------------------
 * 5| == != === !===
 * ------------------
 * 6| && and
 * ------------------
 * 7| || or xor
 * ------------------
 * 8| = += -= *= /= %= ^= <=>
 */

static int operator_priorities[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	5, /* != */
	8, /* = */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	8, /* <=> */
	0,
	5, 5, 5, /* === == !== */
	3, 3, /* + - */
	2, 2, /* * / */
	0, 0,
	8, 8, 8, 8, /* += -= /= *= */
	0,
	4, 4, 4, 4, /* > >= < <= */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	7, /* or */
	6, /* and */
	7, /* xor */
	0, 0,
	1, /* ^ */
	8, 8, /* ^= %= */
	2, /* % */
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0 /* ~ ~~ ~= ~~= */
};

Operator::Operator(Token* token) {

	this->type = token->type;
	this->character = token->content;
	this->priority = operator_priorities[(int) token->type];

//	cout << "operator " << character << " (" << (int)token->type << ") : " << priority << endl;
}

Operator::~Operator() {
}

void Operator::print(std::ostream& os) {
	os << character;
}

}
