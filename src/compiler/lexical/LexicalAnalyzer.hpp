#ifndef LEXICALanalyzer_H_
#define LEXICALanalyzer_H_

#include <vector>
#include <string>
#include "Token.hpp"
#include "LexicalError.hpp"
#include "../resolver/File.hpp"

namespace ls {

enum class LetterType {
	LETTER, NUMBER, QUOTE, DOUBLE_QUOTE, WHITE, OTHER
};

class LexicalAnalyzer {
public:

	File* file;
	std::map<std::string, TokenType> token_map;
	
	LexicalAnalyzer();

	LetterType getLetterType(char32_t c);
	TokenType getTokenType(const std::string& word, TokenType by_default);
	std::vector<Token*> parseTokens(std::string code);

	std::vector<Token*> analyze(File* file);
};

}

#endif
