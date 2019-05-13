#ifndef LEXICALanalyzer_H_
#define LEXICALanalyzer_H_

#include <vector>
#include <string>
#include "Token.hpp"
#include "LexicalError.hpp"

namespace ls {

enum class LetterType {
	LETTER, NUMBER, QUOTE, DOUBLE_QUOTE, WHITE, OTHER
};

class Lexicalanalyzer {
public:

	std::vector<LexicalError> errors;

	LetterType getLetterType(char32_t c);
	TokenType getTokenType(std::string word, TokenType by_default);
	std::vector<Token*> parseTokens(std::string code);

	Lexicalanalyzer();
	std::vector<Token*> analyze(std::string code);
};

}

#endif
