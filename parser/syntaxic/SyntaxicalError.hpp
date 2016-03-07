#ifndef SYNTAXICALERROR_HPP
#define SYNTAXICALERROR_HPP

#include <string>
#include "../lexical/Token.hpp"
using namespace std;

class SyntaxicalError {
public:

	Token* token;
	string message;

	SyntaxicalError(Token* token, string message);
	virtual ~SyntaxicalError();
};

#endif
