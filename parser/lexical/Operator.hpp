#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "../lexical/Token.hpp"

class Operator {
public:

	TokenType type;
	std::string character;
	int priority;

	Operator(Token* token);
	virtual ~Operator();

	void print(std::ostream&);
};

#endif
