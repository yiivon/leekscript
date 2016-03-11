#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include "TokenType.hpp"

class Token {

public:

	TokenType type;
	std::string content;
	unsigned character;
	unsigned line;
	unsigned size;

	Token();
	Token(TokenType type, int line, int character, std::string content);
	virtual ~Token();

	std::string toString();
};

std::ostream& operator << (std::ostream& os, Token& var);

#endif
