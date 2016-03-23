#include "SemanticError.hpp"

using namespace std;

SemanticError::SemanticError(Token* token, string message) {
	this->token = token;
	this->message = message;
}

SemanticError::~SemanticError() {}

