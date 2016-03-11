#include "SemanticError.hpp"

using namespace std;

SemanticError::SemanticError(int line, string message) {
	this->line = line;
	this->message = message;
}

SemanticError::~SemanticError() {}

