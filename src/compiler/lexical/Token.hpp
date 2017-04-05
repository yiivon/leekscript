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

	Token(TokenType type, unsigned int line, unsigned int character, std::string content);
};

std::ostream& operator << (std::ostream& os, Token& var);

}

#endif
