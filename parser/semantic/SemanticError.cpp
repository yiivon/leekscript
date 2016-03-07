#include "SemanticError.hpp"

SemanticError::SemanticError(int line, string message) {
	this->line = line;
	this->message = message;
}

SemanticError::~SemanticError() {}

