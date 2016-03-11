#ifndef SYNTAXICALERROR_HPP
#define SYNTAXICALERROR_HPP

#include <string>
#include "../lexical/Token.hpp"

class SyntaxicalError {
public:

	Token* token;
	std::string message;

	SyntaxicalError(Token* token, std::string message);
	virtual ~SyntaxicalError();
};

#endif
