#include "LexicalError.hpp"
#include "../../util/Util.hpp"

using namespace std;

namespace ls {

bool LexicalError::translation_loaded = false;
Json LexicalError::translation;

LexicalError::LexicalError(Type type, int line, int character)
	: type(type), line(line), character(character) {}

LexicalError::~LexicalError() {}

std::string LexicalError::message() const {
	return build_message(type);
}

Json LexicalError::json() const {
	return {line, character, line, character, build_message(type)};
}

std::string LexicalError::build_message(Type type) {
	if (!translation_loaded) {
		try {
			translation = Json::parse(Util::read_file("src/doc/lexical_exception_fr.json"));
		} catch (exception&) {} // LCOV_EXCL_LINE
		translation_loaded = true;
	}

	try {
		return translation[type_to_string(type)];
	} catch (exception&) { // LCOV_EXCL_LINE
		return type_to_string(type); // LCOV_EXCL_LINE
	}
}

std::string LexicalError::type_to_string(Type type) {
	switch (type) {
		case Type::UNTERMINATED_STRING: return "UNTERMINATED_STRING";
		case Type::UNKNOWN_ESCAPE_SEQUENCE: return "UNKNOWN_ESCAPE_SEQUENCE";
		case Type::NUMBER_INVALID_REPRESENTATION: return "NUMBER_INVALID_REPRESENTATION";
		default:
			return "UNKNOWN_ERROR";
	}
}

}
