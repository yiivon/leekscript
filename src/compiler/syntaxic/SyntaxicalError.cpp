#include "SyntaxicalError.hpp"
#include <iostream>

namespace ls {

SyntaxicalError::SyntaxicalError(const SyntaxicalError& other) {
	this->token = other.token;
	this->type = other.type;
	this->parameters = other.parameters;
}

SyntaxicalError::SyntaxicalError(Token* token, Type type, std::vector<std::string> parameters) {
	this->token = token;
	this->type = type;
	this->parameters = parameters;
}

SyntaxicalError::~SyntaxicalError() {}

std::string SyntaxicalError::message() const {
	return build_message(type, {});
}

Json SyntaxicalError::json() const {
	return {token->location.start.line, token->location.start.column, token->location.end.line, token->location.end.column, build_message(type, parameters)};
}

std::string SyntaxicalError::build_message(Type type, std::vector<std::string>) {
	return std::string("syntaxical_error<") + std::to_string((int) type) + std::string(">");
}

}
