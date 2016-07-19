#ifndef LEXICALANALYSER_H_
#define LEXICALANALYSER_H_

#include <vector>
#include <string>

#include "../../compiler/lexical/Token.hpp"

namespace ls {

enum class LetterType {
	LETTER, NUMBER, QUOTE, DOUBLE_QUOTE, WHITE, OTHER
};

class LexicalAnalyser {

	LetterType getLetterType(char32_t c);
	TokenType getTokenType(std::string word, TokenType by_default);
	std::vector<Token> parseTokens(std::string code);

public:

	LexicalAnalyser();
	std::vector<Token> analyse(std::string code);
};

}

#endif
