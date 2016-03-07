#ifndef SEMANTICERROR_HPP_
#define SEMANTICERROR_HPP_

#include <string>
using namespace std;

class SemanticError {
public:

	int line;
	string message;

	SemanticError(int line, string message);
	virtual ~SemanticError();
};

#endif
