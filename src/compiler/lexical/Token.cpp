#include "Token.hpp"
#include <iostream>

namespace ls {

Token::Token(TokenType type, File* file, size_t raw, size_t line, size_t character, std::string content) : location(file, {line, character - content.size() - 1, raw - content.size() - 1}, {line, character, raw - 1}) {

	this->type = type;
	this->content = std::string(content);

	if (type == TokenType::STRING) {
		this->location.start.column--;
		this->location.start.raw--;
		this->location.end.raw++;
		this->location.end.column++;
		this->size = content.size() + 2;
	} else {
		this->size = content.size();
	}
}

}
