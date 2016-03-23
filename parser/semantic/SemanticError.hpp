#ifndef SEMANTICERROR_HPP_
#define SEMANTICERROR_HPP_

#include <string>
#include "../lexical/Token.hpp"

class SemanticError {
public:

	Token* token;
	std::string message;

	SemanticError(Token* token, std::string message);
	virtual ~SemanticError();
};

#endif
