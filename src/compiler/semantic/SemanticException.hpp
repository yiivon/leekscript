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
		CANT_ASSIGN_VOID,
		CANNOT_CALL_VALUE,
		BREAK_MUST_BE_IN_LOOP,
		CONTINUE_MUST_BE_IN_LOOP,
	};

	static bool translation_loaded;
	static Json translation;
	static std::string type_to_string(Type);
	static std::string build_message(Type, std::string);

	Type type;
	int line;
	std::string content;

	SemanticException(Type type, int line);
	SemanticException(Type type, int line, std::string& content);
	virtual ~SemanticException();

	std::string message() const;
};

}

#endif
