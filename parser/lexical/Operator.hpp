#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "../lexical/Token.hpp"

class Operator {
public:

	TokenType type;
	string character;
	int priority;

	Operator(Token* token);
	virtual ~Operator();

	void print(ostream&);
};

#endif
