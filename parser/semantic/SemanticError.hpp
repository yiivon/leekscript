#ifndef SEMANTICERROR_HPP_
#define SEMANTICERROR_HPP_

#include <string>

class SemanticError {
public:

	int line;
	std::string message;

	SemanticError(int line, std::string message);
	virtual ~SemanticError();
};

#endif
