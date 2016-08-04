#include "LexicalError.hpp"
#include <string>

namespace ls {

LexicalError::LexicalError(Type type, int line, int character)
	: type(type), line(line), character(character) {}

LexicalError::~LexicalError() {}

std::string LexicalError::message() const {
	return build_message(type);
}

std::string LexicalError::build_message(Type type) {
	switch (type) {
	case Type::UNTERMINATED_STRING: return "UNTERMINATED_STRING";
	case Type::UNKNOWN_ESCAPE_SEQUENCE: return "UNKNOWN_ESCAPE_SEQUENCE";
	case Type::NUMBER_INVALID_REPRESENTATION: return "NUMBER_INVALID_REPRESENTATION";
	}
	return "??";
}

}
