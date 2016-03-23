#include "Token.hpp"
#include <iostream>

using namespace std;

Token::Token() {
	character = -1;
	line = -1;
	type = TokenType::UNKNOW;
	size = 0;
}

Token::Token(std::string content) {
	character = -1;
	line = -1;
	type = TokenType::UNKNOW;
	this->content = content;
	size = 0;
}

Token::Token(TokenType type, int line, int character, string content) {

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

Token::~Token() {}

std::ostream& operator << (std::ostream& os, Token& token) {
	os << "[" << token.line << ":" << token.character << " " << (int)token.type << " " << token.content << "]";
	return os;
}
