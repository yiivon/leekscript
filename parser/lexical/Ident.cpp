#include "Ident.hpp"
#include "../../vm/value/LSNull.hpp"

Ident::Ident(Token* token) {
	this->token = token;
}

Ident::~Ident() {}

void Ident::print(ostream& os) {
	os << token->content;
}
