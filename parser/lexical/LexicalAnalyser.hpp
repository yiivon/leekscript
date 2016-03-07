#ifndef LEXICALANALYSER_H_
#define LEXICALANALYSER_H_

#include <vector>
#include <string>
#include "Token.hpp"
using namespace std;

enum class LetterType {
	LETTER, NUMBER, QUOTE, DOUBLE_QUOTE, WHITE, OTHER
};

class LexicalAnalyser {

	LetterType getLetterType(char c);
	vector<Token> parseTokens(string code);

public:

	LexicalAnalyser();
	vector<Token> analyse(string code);

};

#endif
