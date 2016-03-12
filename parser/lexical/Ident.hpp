#ifndef IDENT_HPP_
#define IDENT_HPP_

#include "../lexical/Token.hpp"

class Ident {
public:

	Token* token;

	Ident(Token* token);
	virtual ~Ident();

	void print(std::ostream& os);
};

#endif
