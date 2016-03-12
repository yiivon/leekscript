#include "SyntaxicalError.hpp"

using namespace std;

SyntaxicalError::SyntaxicalError(Token* token, string message) {
	this->token = token;
	this->message = message;
}

SyntaxicalError::~SyntaxicalError() {

}

