#include "../../compiler/lexical/Operator.hpp"

#include <iostream>
using namespace std;

namespace ls {

/*
 * Binary operators precedence
 * ------------------
 * 0| ~ ~~ ~= ~~=
 * ------------------
 * 1| ^
 * ------------------
 * 2| * / %
 * ------------------
 * 3| + -
 * ------------------
 * 4| < <= > >= instanceof
 * ------------------
 * 5| == != === !===
 * ------------------
 * 6| && and
 * ------------------
 * 7| || or xor
 * ------------------
 * 8| ??
 * ------------------
 * 9| = += -= *= /= %= ^= <=>
 */

static int operator_priorities[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	5, /* != */
	9, /* = */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	9, /* <=> */
	0,
	5, 5, 5, /* === == !== */
	3, 3, /* + - */
	2, 2, /* * / */
	0, 0,
	9, 9, 9, 9, /* += -= /= *= */
	0,
	4, 4, 4, 4, /* > >= < <= */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	7, /* or */
	6, /* and */
	7, /* xor */
	0, 0,
	1, /* ^ */
	9, 9, /* ^= %= */
	2, /* % */
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, /* ~ ~~ ~= ~~= */
	0, 0, 0,
	4, /* instanceof */
	8 /* ?? */
};

Operator::Operator(Token* token) {

	this->type = token->type;
	this->character = token->content;
	this->priority = operator_priorities[(int) token->type];

//	cout << "operator " << character << " (" << (int)token->type << ") : " << priority << endl;
}

Operator::~Operator() {}

void Operator::print(std::ostream& os) {
	os << character;
}

}
