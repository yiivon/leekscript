#include "../../compiler/syntaxic/SyntaxicalError.hpp"

using namespace std;

namespace ls {

SyntaxicalError::SyntaxicalError(Token* token, string message) {
	this->token = token;
	this->message = message;
}

SyntaxicalError::~SyntaxicalError() {

}

}
