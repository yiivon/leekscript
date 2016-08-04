#ifndef LEXICAL_ERROR_HPP_
#define LEXICAL_ERROR_HPP_

#include <string>

namespace ls {

class LexicalError {
public:

	enum Type {
		UNTERMINATED_STRING,
		UNKNOWN_ESCAPE_SEQUENCE,
		NUMBER_INVALID_REPRESENTATION
	};

	Type type;
	unsigned int line;
	unsigned int character;

	LexicalError(Type type, int line, int character);
	virtual ~LexicalError();

	std::string message() const;

	static std::string build_message(Type type);
};

}

#endif
