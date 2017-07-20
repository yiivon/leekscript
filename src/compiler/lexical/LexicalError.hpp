#ifndef LEXICAL_ERROR_HPP_
#define LEXICAL_ERROR_HPP_

#include <string>
#include "../../../lib/json.hpp"

namespace ls {

class LexicalError {
public:

	enum Type {
		UNTERMINATED_STRING,
		UNKNOWN_ESCAPE_SEQUENCE,
		NUMBER_INVALID_REPRESENTATION
	};

	static bool translation_loaded;
	static Json translation;
	static std::string type_to_string(Type);
	static std::string build_message(Type);

	Type type;
	unsigned int line;
	unsigned int character;

	LexicalError(Type type, int line, int character);
	virtual ~LexicalError();

	std::string message() const;
	Json json() const;
};

}

#endif
