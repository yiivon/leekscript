#ifndef SYNTAXICANALYSER_HPP
#define SYNTAXICANALYSER_HPP

#include <vector>

#include "../../compiler/instruction/ClassDeclaration.hpp"
#include "../../compiler/instruction/For.hpp"
#include "../../compiler/instruction/Foreach.hpp"
#include "../../compiler/instruction/VariableDeclaration.hpp"
#include "../../compiler/lexical/Token.hpp"
#include "../../compiler/syntaxic/SyntaxicalError.hpp"
#include "../../compiler/value/If.hpp"
#include "../../vm/Program.hpp"
#include "../value/Value.hpp"

namespace ls {

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
	std::vector<SyntaxicalError*> getErrors();
};

}

#endif
