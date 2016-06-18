#include "../../compiler/semantic/SemanticError.hpp"

using namespace std;

namespace ls {

SemanticError::SemanticError(Token* token, string message) {
	this->token = token;
	this->message = message;
}

SemanticError::~SemanticError() {}

}
