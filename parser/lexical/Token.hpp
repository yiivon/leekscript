#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include "TokenType.hpp"
using namespace std;

class Token {

public:

	TokenType type;
	string content;
	unsigned character;
	unsigned line;
	unsigned size;

	Token();
	Token(TokenType type, int line, int character, string content);
	virtual ~Token();

	string toString();
};

std::ostream& operator << (std::ostream& os, Token& var);

#endif
