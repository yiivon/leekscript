#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

#include "../../compiler/lexical/TokenType.hpp"

namespace ls {

class Token {

public:

	TokenType type;
	std::string content;
	unsigned character;
	unsigned line;
	unsigned size;
	bool large_number = false;

	Token(std::string content);
	Token(TokenType type, unsigned int line, unsigned int character, std::string content, bool large_number = false);
	virtual ~Token();
};

std::ostream& operator << (std::ostream& os, Token& var);

}

#endif
