#ifndef TOKENTYPE_H_
#define TOKENTYPE_H_

#include <string>
#include <vector>

namespace ls {

enum class TokenType {

	UNKNOW,
	FINISHED,
	DONT_CARE,

	NUMBER,
	IDENT,
	STRING,

	LET,
	GLOBAL,
	FUNCTION,
	RETURN,
	BREAK,
	CONTINUE,
	DIFFERENT,
	EQUAL,
	NOT,
	NULLL,
	TRUE,
	FALSE,
	COMMENT_START,
	COMMENT_END,
	OPEN_PARENTHESIS,
	CLOSING_PARENTHESIS,
	OPEN_BRACKET,
	CLOSING_BRACKET,
	OPEN_BRACE,
	CLOSING_BRACE,
	SWAP,
	AROBASE,
	TRIPLE_EQUAL,
	DOUBLE_EQUAL,
	TRIPLE_DIFFERENT,
	PLUS,
	MINUS,
	TIMES,
	DIVIDE,
	PLUS_PLUS,
	MINUS_MINUS,
	PLUS_EQUAL,
	MINUS_EQUAL,
	DIVIDE_EQUAL,
	TIMES_EQUAL,
	SEMICOLON,
	GREATER,
	GREATER_EQUALS,
	LOWER,
	LOWER_EQUALS,
	ARROW,
	COMMA,
	COLON,
	IF,
	ELSE,
	MATCH,
	FOR,
	IN,
	WHILE,
	DO,
	QUESTION_MARK,
	OR,
	AND,
	XOR,
	DOT,
	PIPE,
	POWER,
	POWER_EQUAL,
	MODULO_EQUAL,
	MODULO,
	CLASS,
	NEW,
	PARENT,
	THEN,
	END,
	TERNARY,
	NOT_NOT,
	TILDE,
	TILDE_EQUAL,
	TILDE_TILDE,
	TILDE_TILDE_EQUAL,
	PI,
	TWO_DOTS,
	THREE_DOTS,
	INSTANCEOF,
	BIT_AND,
	BIT_AND_EQUALS,
	BIT_OR_EQUALS,
	BIT_XOR,
	BIT_XOR_EQUALS,
	BIT_SHIFT_LEFT,
	BIT_SHIFT_LEFT_EQUALS,
	BIT_SHIFT_RIGHT,
	BIT_SHIFT_RIGHT_EQUALS,
	BIT_SHIFT_RIGHT_UNSIGNED,
	BIT_SHIFT_RIGHT_UNSIGNED_EQUALS,
	BIT_ROTATE_LEFT,
	BIT_ROTATE_LEFT_EQUALS,
	BIT_ROTATE_RIGHT,
	BIT_ROTATE_RIGHT_EQUALS,
	DOUBLE_QUESTION_MARK,
	INT_DIV,
	THROW,
	VAR,
	INT_DIV_EQUAL
};

}

#endif
