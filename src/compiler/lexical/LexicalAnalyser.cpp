#include "../../compiler/lexical/LexicalAnalyser.hpp"

#include <iostream>
#include "../../../lib/utf8.h"
#include <codecvt>
#include <locale>
#include <string.h>

using namespace std;

namespace ls {

static vector<string> type_literals[] = {

	{ "" }, // Unknown
	{ "" }, // Finished
	{ "" }, // Don't care

	{ "" }, // Number
	{ "" }, // Ident
	{ "" }, // String

	{ "let", "var" }, { "global" }, { "function" }, { "return" },
	{ "break" }, { "continue" },

	{ "!=", "is not", "≠" }, { "=" },

	{ "!", "not" }, { "null" }, { "true" }, { "false" }, { "/*" },
	{ "*/" }, { "(" }, { ")" }, { "[" }, { "]" }, { "{" }, { "}" },

	{ "<=>", "<->" },

	{ "@" },

	{ "===" }, { "==", "is" }, { "!==" },
	{ "+" }, { "-" }, { "*", "×" }, { "/", "÷" },

	{ "++" }, { "--" },

	{ "+=" }, {"-=" }, { "/=", "÷=" }, { "*=", "×=" },

	{ ";" },

	{ ">" }, {	">=", "≥" }, { "<" }, { "<=", "≤" },

	{ "->", "=>", "→" }, { "," }, { ":" }, { "if" }, { "else" }, { "for" }, { "in" },
	{ "while" }, { "do" }, { "?" },

	{ "or", "||" }, { "and", "&&" }, { "xor" },

	{ "." }, { "|" },

	{ "^", "**" }, { "^=", "**=" }, { "%=" }, {"%" },

	{ "class" }, { "new" }, { "parent" }, { "then" },
	{ "end" }, { "?:" }, { "!!" },

	{ "~" }, { "~=" }, { "~~" }, { "~~=" },

	{ "π" },

	{ ".." }, { "..." }
};

LexicalAnalyser::LexicalAnalyser() {}

LetterType LexicalAnalyser::getLetterType(char32_t c) {

	if (c == '\'') {
		return LetterType::QUOTE;
	}
	if (c == '"') {
		return LetterType::DOUBLE_QUOTE;
	}
	if (c >= '0' && c <= '9') {
		return LetterType::NUMBER;
	}
	if (c == ' ' || c == '	' || c == '\n') {
		return LetterType::WHITE;
	}
	if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c >= 0xc0) {
		return LetterType::LETTER;
	}

	return LetterType::OTHER;
}

vector<Token> LexicalAnalyser::analyse(std::string code) {

	vector<Token> tokens = LexicalAnalyser::parseTokens(code + " ");

	tokens.push_back(Token(TokenType::FINISHED, 0, 1, ""));

	for (unsigned i = 0; i < tokens.size(); ++i) {

		Token& token = tokens.at(i);

		if (i < tokens.size() - 1 && token.content == "is" && tokens.at(i + 1).content == "not") {
			token.type = TokenType::DIFFERENT;
			token.content = "is not";
			tokens.erase(tokens.begin() + i + 1);
		}

		if (token.type == TokenType::UNKNOW || token.type == TokenType::IDENT) {

			int i = 0;
			for (auto type : type_literals) {
				for (string text : type) {
					if (text.size() > 0 && token.content == text) {
						token.type = (TokenType) i;
						break;
					}
				}
				i++;
			}
		}
	}

	return tokens;
}

vector<Token> LexicalAnalyser::parseTokens(string code) {

	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

	char* string_chars = (char*) code.c_str();

	vector<Token> tokens;

	int line = 1;
	int character = 1;
	string word = "";
	bool ident = false;
	bool number = false;
	bool string1 = false;
	bool string2 = false;
	bool other = false;
	int comment = 0;
	bool lineComment = false;

	int i = 0;
	int l = strlen(string_chars);

	while (i < l) {

		character++;

		int c = u8_nextchar(string_chars, &i);
		int oldi = i;
		int nc = u8_nextchar(string_chars, &i);
		i = oldi;

		LetterType type = getLetterType(c);

		if (lineComment) {
			if (c == '\n') {
				lineComment = false;
			}
		} else {

			if (c == '/' && nc == '/') {
				lineComment = true;
			} else if (c == '/' && nc == '*') {
				comment++;
				i++;
			} else if (c == '*' && nc == '/') {
				comment--;
				i++;

			} else if (comment == 0) {

				if (type == LetterType::WHITE) {

					if (ident) {
						tokens.push_back(Token(TokenType::IDENT, line, character, word));
						ident = false;
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
					} else if (string1 || string2) {
						word += converter.to_bytes(c);
					} else if (other) {
						tokens.push_back(Token(TokenType::UNKNOW, line, character, word));
						other = false;
					}

				} else if (type == LetterType::LETTER) {
					if (ident || string1 || string2) {
						word += converter.to_bytes(c);
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
					} else if (other) {
						tokens.push_back(Token(TokenType::UNKNOW, line, character, word));
						other = false;
						ident = true;
						word = converter.to_bytes(c);
					} else {
						ident = true;
						word = converter.to_bytes(c);
					}
				} else if (type == LetterType::NUMBER) {
					if (number || ident || string1 || string2) {
						word += converter.to_bytes(c);
					} else if (other) {
						tokens.push_back(Token(TokenType::UNKNOW, line, character, word));
						other = false;
						number = true;
						word = converter.to_bytes(c);
					} else {
						number = true;
						word = converter.to_bytes(c);
					}
				} else if (type == LetterType::QUOTE) {
					if (ident) {
						tokens.push_back(Token(TokenType::IDENT, line, character, word));
						ident = false;
						string1 = true;
						word = "";
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
						string1 = true;
						word = "";
					} else if (string1) {
						tokens.push_back(Token(TokenType::STRING, line, character, word));
						string1 = false;
					} else if (string2) {
						word += converter.to_bytes(c);
					} else if (other) {
						tokens.push_back(Token(TokenType::UNKNOW, line, character, word));
						other = false;
						string1 = true;
						word = "";
					} else {
						string1 = true;
						word = "";
					}
				} else if (type == LetterType::DOUBLE_QUOTE) {
					if (ident) {
						tokens.push_back(Token(TokenType::IDENT, line, character, word));
						ident = false;
						string2 = true;
						word = "";
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
						string2 = true;
						word = "";
					} else if (string1) {
						word += converter.to_bytes(c);
					} else if (string2) {
						tokens.push_back(Token(TokenType::STRING, line, character, word));
						string2 = false;
					} else if (other) {
						tokens.push_back(Token(TokenType::UNKNOW, line, character, word));
						other = false;
						string2 = true;
						word = "";
					} else {
						string2 = true;
						word = "";
					}
				} else if (type == LetterType::OTHER) {
					if (ident) {
						tokens.push_back(Token(TokenType::IDENT, line, character, word));
						ident = false;
						other = true;
						word = converter.to_bytes(c);
					} else if (number) {
						if (c == '.' && word.find('.') == string::npos && getLetterType(nc) == LetterType::NUMBER) {
							word += converter.to_bytes(c);
						} else {
							tokens.push_back(Token(TokenType::NUMBER, line, character, word));
							number = false;
							other = true;
							word = converter.to_bytes(c);
						}
					} else if (string1 || string2) {
						word += converter.to_bytes(c);
					} else if (other) {

						if (c == '.' && (word == "." || word == "..")) {
							word += converter.to_bytes(c);

						} else if (string("([{}]),;.").find(c) != string::npos || string("([{}]),;.").find(word) != string::npos
								|| (word == "!" && c == '!')) {

							tokens.push_back(Token(TokenType::UNKNOW, line, character, word));
							word = converter.to_bytes(c);
						} else {
							word += converter.to_bytes(c);
						}
					} else {
						word = converter.to_bytes(c);
						other = true;
					}
				}
			}
		}
		if (c == '\n') {
			line++;
			character = 0;
		}
	}
	return tokens;
}

}
