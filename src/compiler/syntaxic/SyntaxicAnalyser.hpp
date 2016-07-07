#ifndef SYNTAXICANALYSER_HPP
#define SYNTAXICANALYSER_HPP

#include <vector>

#include "SyntaxicalError.hpp"

namespace ls {

class Token;
enum class TokenType;
class Ident;
class Instruction;
class Value;
class ClassDeclaration;
class VariableDeclaration;
class If;
class For;
class Foreach;
class Program;
class Block;
class Object;
class Array;

class SyntaxicAnalyser {

	std::vector<Token> tokens;
	Token* t;
	Token* lt;
	Token* nt;
	unsigned i;
	std::vector<SyntaxicalError*> errors;
	long time;

public:

	SyntaxicAnalyser();
	~SyntaxicAnalyser();

	Program* analyse(std::vector<Token>&);

	Token* eat();
	Token* eat(TokenType type);
	Token* nextTokenAt(int pos);

	Block* eatMain();
	Ident* eatIdent();
	Value* eatExpression();
	Value* eatSimpleExpression();
	Value* eatValue();
	bool isObject();
	Value* eatBlockOrObject();
	Block* eatBlock();
	Object* eatObject();
	Array* eatArray();
	If* eatIf();
	Instruction* eatFor();
	Instruction* eatWhile();
	Foreach* eatForeach();
	ClassDeclaration* eatClassDeclaration();
	VariableDeclaration* eatVariableDeclaration();
	Instruction* eatInstruction();

	long getTime();
	std::vector<SyntaxicalError*> getErrors();
};

}

#endif
