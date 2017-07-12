#include "SyntaxicalError.hpp"
#include "../../util/Util.hpp"

namespace ls {

bool SyntaxicalError::translation_loaded = false;
Json SyntaxicalError::translation;

SyntaxicalError::SyntaxicalError(const SyntaxicalError& other) {
	this->token = other.token;
	this->type = other.type;
	this->parameters = other.parameters;
}

SyntaxicalError::SyntaxicalError(Token* token, Type type, std::vector<std::string> parameters) {
	this->token = token;
	this->type = type;
	this->parameters = parameters;
}

SyntaxicalError::~SyntaxicalError() {}

std::string SyntaxicalError::message() const {
	return build_message(type, parameters);
}

Json SyntaxicalError::json() const {
	return {token->location.start.line, token->location.start.column, token->location.end.line, token->location.end.column, build_message(type, parameters)};
}

std::string SyntaxicalError::build_message(Type type, std::vector<std::string> parameters) {
	if (!translation_loaded) {
		try {
			translation = Json::parse(Util::read_file("src/doc/syntaxical_exception_fr.json"));
		} catch (std::exception&) {} // LCOV_EXCL_LINE
		translation_loaded = true;
	}

	try {
		std::string m = translation[type_to_string(type)];
		size_t pos = 0;
		size_t i = 0;
		while ((pos = m.find("%", pos + 1)) != std::string::npos) {
			if (i < parameters.size()) {
				m = m.replace(pos, 2, parameters[i]);
			}
			i++;
		}
		return m;
	} catch (std::exception&) { // LCOV_EXCL_LINE
		return type_to_string(type); // LCOV_EXCL_LINE
	}
}

std::string SyntaxicalError::type_to_string(Type type) {
	switch (type) {
		case Type::BLOCK_NOT_CLOSED: return "BLOCK_NOT_CLOSED";
		case Type::BREAK_LEVEL_ZERO: return "BREAK_LEVEL_ZERO";
		case Type::CONTINUE_LEVEL_ZERO: return "CONTINUE_LEVEL_ZERO";
		case Type::EXPECTED_VALUE: return "EXPECTED_VALUE";
		case Type::UNEXPECTED_TOKEN: return "UNEXPECTED_TOKEN";
		default:
			return "UNKNOWN_ERROR";
	}
}

}