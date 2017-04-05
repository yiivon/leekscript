#include "../../compiler/lexical/Token.hpp"

#include <iostream>

using namespace std;

namespace ls {

Token::Token(TokenType type, unsigned int line, unsigned int character, string content) {

	this->type = type;
	this->character = character - content.size() - 1;
	this->line = line;
	this->content = string(content);

	if (type == TokenType::STRING) {
		this->character--;
		this->size = content.size() + 2;
	} else {
		this->size = content.size();
	}
}

}
