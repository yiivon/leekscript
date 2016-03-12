#ifndef LEXICALANALYSER_H_
#define LEXICALANALYSER_H_

#include <vector>
#include <string>
#include "Token.hpp"

enum class LetterType {
	LETTER, NUMBER, QUOTE, DOUBLE_QUOTE, WHITE, OTHER
};

class LexicalAnalyser {

	LetterType getLetterType(char c);
	std::vector<Token> parseTokens(std::string code);

public:

	LexicalAnalyser();
	std::vector<Token> analyse(std::string code);

};

#endif
