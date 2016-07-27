#include "LexicalError.hpp"
#include <string>

LexicalError::LexicalError(Type type, int line, int character)
	: type(type), line(line), character(character) {}

LexicalError::~LexicalError() {}

std::string LexicalError::message() const {
	switch (type) {
	case Type::UNTERMINATED_STRING: return "UNTERMINATED_STRING";
	}
	return "??";
}
