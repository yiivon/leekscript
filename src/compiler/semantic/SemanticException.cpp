#include "SemanticException.hpp"
#include "../../util/Util.hpp"

using namespace std;

namespace ls {

bool SemanticException::translation_loaded = false;
Json SemanticException::translation;

SemanticException::SemanticException(Type type, Token* token) {
	this->type = type;
	this->token = token;
}

SemanticException::~SemanticException() {}

std::string SemanticException::message() const {

	return build_message(type, token->content);
}

std::string SemanticException::build_message(Type type, std::string token) {

	if (!translation_loaded) {
		translation = Json::parse(Util::read_file("src/doc/semantic_exception_fr.json"));
		translation_loaded = true;
	}

	std::string raw = translation[type_to_string(type)];
	std::string m = raw.replace(raw.find("%s"), 2, token);
	return m;
}

std::string SemanticException::type_to_string(Type type) {
	switch (type) {
		case Type::UNDEFINED_VARIABLE: return "UNDEFINED_VARIABLE";
		case Type::VARIABLE_ALREADY_DEFINED: return "VARIABLE_ALREADY_DEFINED";
		case Type::METHOD_NOT_FOUND: return "METHOD_NOT_FOUND";
		case Type::STATIC_METHOD_NOT_FOUND: return "STATIC_METHOD_NOT_FOUND";
	}
	return "?";
}

}

