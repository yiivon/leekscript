#ifndef SYNTAXICANALYSER_HPP
#define SYNTAXICANALYSER_HPP

#include <vector>
#include "../lexical/Token.hpp"
#include "../Program.hpp"
#include "SyntaxicalError.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../value/Value.hpp"
#include "../value/If.hpp"
#include "../instruction/For.hpp"
#include "../instruction/Foreach.hpp"
#include "../instruction/ClassDeclaration.hpp"

class SyntaxicAnalyser {

	vector<Token> tokens;
	Token* t;
	Token* lt;
	Token* nt;
	unsigned i;
	vector<SyntaxicalError*> errors;
	long time;

public:

	SyntaxicAnalyser();
	~SyntaxicAnalyser();

	Program* analyse(vector<Token>&);

	Token* eat();
	Token* eat(TokenType type);
	Token* nextTokenAt(int pos);

	Ident* eatIdent();
	Value* eatExpression();
	Value* eatSimpleExpression();
	Value* eatValue();
	Body* eatBody();
	If* eatIf();
	Instruction* eatFor();
	Instruction* eatWhile();
	Foreach* eatForeach();
	ClassDeclaration* eatClassDeclaration();
	VariableDeclaration* eatVariableDeclaration();
	Instruction* eatInstruction();

	long getTime();
	vector<SyntaxicalError*> getErrors();
};

#endif
