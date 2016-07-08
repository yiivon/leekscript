#ifndef SEMANTICERROR_HPP_
#define SEMANTICERROR_HPP_

#include <string>
#include "../lexical/Token.hpp"
#include "../../../lib/json.hpp"

namespace ls {

class SemanticException : public std::exception {
public:

	enum Type {
		UNDEFINED_VARIABLE,
		VARIABLE_ALREADY_DEFINED,
		METHOD_NOT_FOUND,
		STATIC_METHOD_NOT_FOUND,
		CANT_ASSIGN_VOID
	};

	static bool translation_loaded;
	static Json translation;
	static std::string type_to_string(Type);
	static std::string build_message(Type, std::string);

	Type type;
	Token* token;

	SemanticException(Type type, Token* token);
	virtual ~SemanticException();

	std::string message() const;
};

}

#endif
