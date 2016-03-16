#include <string>
#include "SyntaxicAnalyser.hpp"
#include "../value/AbsoluteValue.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../instruction/Return.hpp"
#include "../value/Object.hpp"
#include "../value/PrefixExpression.hpp"
#include "../value/PostfixExpression.hpp"
#include "../value/FunctionCall.hpp"
#include "../value/ArrayAccess.hpp"
#include "../value/ObjectAccess.hpp"
#include "../value/Number.hpp"
#include "../value/Boolean.hpp"
#include "../instruction/While.hpp"
#include "../value/String.hpp"
#include "../value/Array.hpp"
#include "../value/Function.hpp"
#include "../value/Reference.hpp"
#include "../value/VariableValue.hpp"
#include "../value/Nulll.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Continue.hpp"
#include "../instruction/Instruction.hpp"
#include <math.h>

using namespace std;

SyntaxicAnalyser::SyntaxicAnalyser() {
	time = 0;
	lt = nullptr;
	nt = nullptr;
	t = nullptr;
	i = 0;
}

SyntaxicAnalyser::~SyntaxicAnalyser() {
	for (SyntaxicalError* error : errors) {
		delete error;
	}
}

Program* SyntaxicAnalyser::analyse(vector<Token>& tokens) {

	this->tokens = tokens;
	this->lt = nullptr;
	this->t = &tokens.at(0);
	this->nt = nullptr;
	this->i = 0;

//	time = System.nanoTime();

	Program* program = new Program();
	program->body = eatBody();

//	if (program->body->instructions.size() > 0) {
//		Instruction* last = program->body->instructions[program->body->instructions.size() - 1];
//		if (dynamic_cast<Return*>(last) == nullptr) {
//			if (dynamic_cast<ExpressionInstruction*>(last) != nullptr) {
//				Return* ret = new Return();
//				ret->expression = ((ExpressionInstruction*) last)->expression;
//				program->body->instructions[program->body->instructions.size() - 1] = ret;
//			} else {
//				Return* ret = new Return();
//				ret->expression = new Nulll();
//				program->body->instructions.push_back(ret);
//			}
//		}
//	}
//
//	if (program->body->instructions.size() == 0) {
//		Return* ret = new Return();
//		ret->expression = new Nulll();
//		program->body->instructions.push_back(ret);
//	}

//	time = System.nanoTime() - time;

	return program;
}

Body* SyntaxicAnalyser::eatBody() {

	Body* body = new Body();

	Instruction* ins;
	while ((ins = eatInstruction()) != nullptr) {
		body->instructions.push_back(ins);
	}

	return body;
}

Instruction* SyntaxicAnalyser::eatInstruction() {

	while (t->type == TokenType::SEMICOLON) {
		eat();
	}

	switch (t->type) {

		case TokenType::LET:
		case TokenType::GLOBAL: {
			return eatVariableDeclaration();
		}
		case TokenType::NUMBER:
		case TokenType::PI:
		case TokenType::TRUE:
		case TokenType::FALSE:
		case TokenType::STRING:
		case TokenType::IDENT:
		case TokenType::OPEN_BRACKET:
		case TokenType::OPEN_PARENTHESIS:
		case TokenType::OPEN_BRACE:
		case TokenType::NULLL:
		case TokenType::ARROW:
		case TokenType::IF:
		case TokenType::MINUS:
		case TokenType::NOT:
		case TokenType::MINUS_MINUS:
		case TokenType::PLUS_PLUS:
		case TokenType::NEW:
		case TokenType::AROBASE:
		case TokenType::PLUS:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::MODULO:
		case TokenType::PIPE:
		case TokenType::FUNCTION:
		case TokenType::TILDE:
		{
			return new ExpressionInstruction(eatExpression());
		}

		case TokenType::RETURN: {

			eat();
			Return* r = new Return();
			r->expression = eatExpression();
			return r;
		}

		case TokenType::BREAK: {

			eat();
			Break* r = new Break();
			return r;
		}

		case TokenType::CONTINUE: {

			eat();
			Continue* r = new Continue();
			return r;
		}

		case TokenType::CLASS: {
			return eatClassDeclaration();
		}

		case TokenType::FOR: {
			return eatFor();
		}

		case TokenType::WHILE: {
			return eatWhile();
		}

		case TokenType::CLOSING_BRACE:
		case TokenType::FINISHED:
		case TokenType::ELSE:
		case TokenType::END:
			return nullptr;

		default:
			errors.push_back(new SyntaxicalError(t, "Unexpected token <" + to_string((int)t->type) + "> (" + t->content + ")"));
			eat();
			return nullptr;
	}
}

VariableDeclaration* SyntaxicAnalyser::eatVariableDeclaration() {

	VariableDeclaration* vd = new VariableDeclaration();

	if (t->type == TokenType::GLOBAL) {
		vd->global = true;
		eat(TokenType::GLOBAL);
	} else {
		eat(TokenType::LET);
	}

	if (t->type == TokenType::NEW) {
		vd->variables.push_back(t->content);
		eat();
	} else {
		vd->variables.push_back(eatIdent()->token->content);
	}

	while (t->type == TokenType::COMMA) {

		eat(TokenType::COMMA);

		if (t->type == TokenType::NEW) {
			vd->variables.push_back(t->content);
			eat();
		} else {
			vd->variables.push_back(eatIdent()->token->content);
		}
	}

	if (t->type == TokenType::EQUAL) {

		eat(TokenType::EQUAL);

		vd->expressions.push_back(eatExpression());

		while (t->type == TokenType::COMMA) {
			eat();
			vd->expressions.push_back(eatExpression());
		}
	}

	return vd;
}

Value* SyntaxicAnalyser::eatSimpleExpression() {

	Value* e = new Expression();

	if (t->type == TokenType::OPEN_PARENTHESIS) {

		eat();
		e = eatExpression();
		e->parenthesis = true;
		eat(TokenType::CLOSING_PARENTHESIS);

	} else if (t->type == TokenType::PIPE) {

		eat();
		AbsoluteValue* av = new AbsoluteValue();
		av->expression = eatExpression();
		eat(TokenType::PIPE);
		e = new Expression(av);

	} else {

		// Opérateurs unaires préfixe
		if (t->type == TokenType::NEW || t->type == TokenType::MINUS ||
				t->type == TokenType::NOT || t->type == TokenType::MINUS_MINUS ||
				t->type == TokenType::PLUS_PLUS || t->type == TokenType::TILDE) {

			if (t->type == TokenType::MINUS && nt != nullptr && t->line != nt->line) {

				e = new Expression(eatValue());

			} else {

				Token* op = eat();
				PrefixExpression* ex = new PrefixExpression();

				ex->operatorr = new Operator(op);
				ex->expression = eatSimpleExpression();

				((Expression*) e)->v1 = ex;
			}

		} else {

			e = eatValue();
		}
	}

	while (t->type == TokenType::OPEN_BRACKET || t->type == TokenType::OPEN_PARENTHESIS
			|| t->type == TokenType::DOT) {

		if (t->character != lt->character + lt->size)
			break;

		switch (t->type) {

			case TokenType::OPEN_BRACKET: {

				ArrayAccess* aa = new ArrayAccess();
				eat(TokenType::OPEN_BRACKET);

				aa->array = e;
				aa->key = eatExpression();

				if (t->type == TokenType::COLON) {
					eat();
					aa->key2 = eatExpression();
				}

				eat(TokenType::CLOSING_BRACKET);
				e = aa;
				break;
			}

			case TokenType::OPEN_PARENTHESIS: {

				FunctionCall* fc = new FunctionCall();
				eat(TokenType::OPEN_PARENTHESIS);

				fc->function = e;

				while (t->type != TokenType::FINISHED and t->type != TokenType::CLOSING_PARENTHESIS) {
					fc->arguments.push_back(eatExpression());
					if (t->type == TokenType::COMMA) {
						eat(TokenType::COMMA);
					}
				}
				eat(TokenType::CLOSING_PARENTHESIS);

				e = fc;
				break;
			}

			case TokenType::DOT: {

				ObjectAccess* oa = new ObjectAccess();
				eat(TokenType::DOT);

				oa->object = e;

				if (t->type == TokenType::NEW || t->type == TokenType::CLASS) {
					oa->field = t->content;
					eat();
				} else if (t->type == TokenType::NUMBER) {
					oa->field = t->content;
//					oa->isNumber = true;
					eat();
				} else {
					Ident* ident = eatIdent();
					oa->field = ident->token->content;
				}
				e = oa;
				break;
			}

			default:
				break;
		}
	}

	// Opérateurs unaires postfixes
	if (t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS) {

		if (lt->line == t->line) {

			Token* op = eat();
			PostfixExpression* ex = new PostfixExpression();

			ex->operatorr = new Operator(op);
			ex->expression = (LeftValue*) e;

			e = ex;
		}
	}

	return e;
}

Value* SyntaxicAnalyser::eatExpression() {

	Expression* ex = nullptr;
	Value* e = eatSimpleExpression();

	// Opérateurs binaires
	while (t->type == TokenType::PLUS || t->type == TokenType::MINUS ||
			t->type == TokenType::TIMES || t->type == TokenType::DIVIDE ||
			t->type == TokenType::MODULO || t->type == TokenType::AND ||
			t->type == TokenType::OR || t->type == TokenType::XOR ||
			t->type == TokenType::EQUAL || t->type == TokenType::POWER ||
			t->type == TokenType::DOUBLE_EQUAL || t->type == TokenType::DIFFERENT ||
			t->type == TokenType::TRIPLE_EQUAL || t->type == TokenType::TRIPLE_DIFFERENT ||
			t->type == TokenType::GREATER || t->type == TokenType::LOWER ||
			t->type == TokenType::GREATER_EQUALS || t->type == TokenType::LOWER_EQUALS ||
			t->type == TokenType::TIMES_EQUAL || t->type == TokenType::PLUS_EQUAL ||
			t->type == TokenType::MINUS_EQUAL || t->type == TokenType::DIVIDE_EQUAL ||
			t->type == TokenType::MODULO_EQUAL || t->type == TokenType::POWER_EQUAL ||
			t->type == TokenType::SWAP || t->type == TokenType::TILDE ||
			t->type == TokenType::TILDE_TILDE || t->type == TokenType::TILDE_EQUAL ||
			t->type == TokenType::TILDE_TILDE_EQUAL || t->type == TokenType::IN) {

		if (t->type == TokenType::MINUS && t->line != lt->line && nt != nullptr && t->line == nt->line)
			break;

		Operator* op = new Operator(t);
		eat();

		if (ex == nullptr) {
			if (Expression* exx = dynamic_cast<Expression*>(e)) {
				ex = exx;
			} else {
				ex = new Expression();
				ex->v1 = e;
			}
		}
		ex->append(op, eatSimpleExpression());
	}

	if (ex != nullptr) {
		return ex;
	}
	return e;
}

Value* SyntaxicAnalyser::eatValue() {

	switch (t->type) {

		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MODULO:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::TERNARY: {

			VariableValue* v = new VariableValue(t);
			eat();
			return v;
		}

		case TokenType::NUMBER: {
			Number* n = new Number(stod(t->content));
			eat();
			return n;
		}
		case TokenType::PI: {
			Number* n = new Number(M_PI);
			eat();
			return n;
		}
		case TokenType::STRING: {
			String* v = new String(t->content);
			eat();
			return v;
		}
		case TokenType::TRUE:
		case TokenType::FALSE: {
			Boolean* bv = new Boolean(t->content == "true");
			eat();
			return bv;
		}
		case TokenType::NULLL: {
			eat();
			return new Nulll();
		}

		case TokenType::IDENT: {

			Ident* ident = eatIdent();

			switch (t->type) {

				case TokenType::ARROW: {

					Function* l = new Function();
					l->lambda = true;
					l->arguments.push_back(ident->token->content);
					eat(TokenType::ARROW);
					l->body = new Body();
					l->body->instructions.push_back(new Return(eatExpression()));

					return l;
				}

				case TokenType::COMMA: {

					// Là y'a une virgule, il faut voir si y'a une flèche
					// plus loin
					bool canBeLamda = true;
					int pos = i;
					while (true) {
						if (tokens.at(pos).type == TokenType::ARROW) {
							break;
						}
						if (tokens.at(pos).type != TokenType::COMMA || tokens.at(pos + 1).type != TokenType::IDENT) {
							canBeLamda = false;
							break;
						}
						pos += 2;
					}

					if (canBeLamda) {

						Function* l = new Function();
						l->lambda = true;
						l->arguments.push_back(ident->token->content);
						while (t->type == TokenType::COMMA) {
							eat();
							l->arguments.push_back(eatIdent()->token->content);
						}

						eat(TokenType::ARROW);
						l->body = new Body();
						l->body->instructions.push_back(new Return(eatExpression()));

						return l;

					} else {
						return new VariableValue(ident->token);
					}
				}
				default: {
					return new VariableValue(ident->token);
				}
			}
			break;
		}

		case TokenType::AROBASE: {
			eat();
			Reference* r = new Reference();
			r->variable = eatIdent()->token->content;
			return r;
		}

		case TokenType::OPEN_BRACKET: {
			eat();
			Array* a = new Array();

			while (t->type != TokenType::CLOSING_BRACKET) {

				Value* key = nullptr;
				Value* value = eatExpression();

				if (t->type == TokenType::COLON) {
					eat(TokenType::COLON);
					key = value;
					value = eatExpression();
				}

				a->addValue(value, key);

				if (t->type == TokenType::COMMA) {
					eat();
				} else {
					break;
				}
			}
			eat(TokenType::CLOSING_BRACKET);

			return a;
		}

		case TokenType::OPEN_BRACE: {

			eat();
			Object* od = new Object();

			while (t->type == TokenType::IDENT) {
				od->keys.push_back(eatIdent());
				eat(TokenType::COLON);
				od->values.push_back(eatExpression());

				if (t->type == TokenType::COMMA) {
					eat();
				}
			}
			eat(TokenType::CLOSING_BRACE);

			return od;
		}

		case TokenType::IF: {
			return eatIf();
		}

		case TokenType::FUNCTION: {

			eat();

			Function* f = new Function();

			eat(TokenType::OPEN_PARENTHESIS);

			while (t->type != TokenType::FINISHED and t->type != TokenType::CLOSING_PARENTHESIS) {



				bool reference = false;
				if (t->type == TokenType::AROBASE) {
					eat();
					reference = true;
				}

				Ident* ident = eatIdent();

				Value* defaultValue = nullptr;
				if (t->type == TokenType::EQUAL) {
					eat();
					defaultValue = eatExpression();
				}

				f->addArgument(ident->token->content, reference, defaultValue);
				if (t->type == TokenType::COMMA) {
					eat();
				}
			}
			eat(TokenType::CLOSING_PARENTHESIS);

			bool braces = false;
			if (t->type == TokenType::OPEN_BRACE) {
				eat(TokenType::OPEN_BRACE);
				braces = true;
			}

			f->body = eatBody();

			if (braces)
				eat(TokenType::CLOSING_BRACE);
			else
				eat(TokenType::END);

			return f;
		}

		case TokenType::ARROW: {

			Function* l = new Function();
			l->lambda = true;
			eat(TokenType::ARROW);
			l->body = new Body();
			l->body->instructions.push_back(new Return(eatExpression()));
			return l;
		}
		default: {}
	}

	errors.push_back(new SyntaxicalError(t, "Expected value, got <" + to_string((int)t->type) + "> (" + t->content + ")"));
	eat();
	return new Nulll();
}

If* SyntaxicAnalyser::eatIf() {

	If* iff = new If();

	eat(TokenType::IF);

	iff->condition = eatExpression();

	bool braces = false;
	bool then = false;
	if (t->type == TokenType::OPEN_BRACE) {
		eat(TokenType::OPEN_BRACE);
		braces = true;
	} else if (t->type == TokenType::THEN) {
		eat(TokenType::THEN);
		then = true;
	}

	if (then or braces) {
		iff->then = eatBody();
	} else {
		Body* body = new Body();
		body->instructions.push_back(eatInstruction());
		iff->then = body;
	}

	if (braces) {
		eat(TokenType::CLOSING_BRACE);
	} else if (then) {
		if (t->type != TokenType::ELSE) {
			eat(TokenType::END);
		}
	}

	if (t->type == TokenType::ELSE) {
		eat();

		bool bracesElse = false;
		if (t->type == TokenType::OPEN_BRACE) {
			eat(TokenType::OPEN_BRACE);
			bracesElse = true;
		}

		if (then or bracesElse) {
			iff->elze = eatBody();
		} else {
			Body* body = new Body();
			body->instructions.push_back(eatInstruction());
			iff->elze = body;
		}

		if (bracesElse) {
			eat(TokenType::CLOSING_BRACE);
		} else if (then) {
			eat(TokenType::END);
		}
	}

	return iff;
}

Instruction* SyntaxicAnalyser::eatFor() {

	eat(TokenType::FOR);

	bool parenthesis = false;
	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	bool forEach = true;
	if (t->type == TokenType::SEMICOLON) {
		forEach = false;
	} else {
		if (t->type == TokenType::LET)
			eat();

		Token* t3 = nextTokenAt(3);
		Token* t4 = nextTokenAt(4);
		forEach = (nt != nullptr && (nt->type == TokenType::COLON || nt->type == TokenType::IN))
				|| (t3 != nullptr && t3->type == TokenType::IN) || (t4 != nullptr && t4->type == TokenType::IN);
	}

	if (forEach) {

		Foreach* f = new Foreach();

		if (nt->type == TokenType::COMMA || nt->type == TokenType::COLON) {
			f->key = eatIdent()->token->content;
			eat();
		}
		if (t->type == TokenType::LET)
			eat();

		f->value = eatIdent()->token->content;

		eat(TokenType::IN);

		f->array = eatExpression();

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		bool braces = false;
		if (t->type == TokenType::OPEN_BRACE) {
			eat();
			braces = true;
		} else {
			eat(TokenType::DO);
		}

		f->body = eatBody();

		if (braces) {
			eat(TokenType::CLOSING_BRACE);
		} else {
			eat(TokenType::END);
		}

		return f;

	} else {

		For* f = new For();

		while (t->type != TokenType::SEMICOLON) {
			if (t->type == TokenType::LET)
				eat();
			f->variables.push_back(eatIdent()->token->content);
			if (t->type == TokenType::EQUAL) {
				eat();
				f->variablesValues.push_back(eatExpression());
			} else {
				f->variablesValues.push_back(nullptr);
			}
			if (t->type == TokenType::COMMA) {
				eat();
			}
		}

		eat(TokenType::SEMICOLON);

		if (t->type != TokenType::SEMICOLON) {
			f->condition = eatExpression();
		}

		eat(TokenType::SEMICOLON);

		while (parenthesis ? (t->type != TokenType::CLOSING_PARENTHESIS) :
				(t->type != TokenType::DO && t->type != TokenType::OPEN_BRACE)) {

			if (t->type != TokenType::FINISHED) {
//				System.out.println(t);
			}
			f->iterations.push_back(eatExpression());
			if (t->type == TokenType::COMMA)
				eat();
		}

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		bool braces = false;
		if (t->type == TokenType::OPEN_BRACE) {
			eat();
			braces = true;
		} else {
			eat(TokenType::DO);
		}

		f->body = eatBody();

		if (braces) {
			eat(TokenType::CLOSING_BRACE);
		} else {
			eat(TokenType::END);
		}

		return f;
	}
}

Instruction* SyntaxicAnalyser::eatWhile() {

	eat(TokenType::WHILE);

	While* w = new While();

	bool parenthesis = false;
	bool braces = false;

	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	w->condition = eatExpression();

	if (parenthesis) {
		eat(TokenType::CLOSING_PARENTHESIS);
	}
	if (t->type == TokenType::OPEN_BRACE) {
		eat();
		braces = true;
	} else {
		eat(TokenType::DO);
	}

	w->body = eatBody();

	if (braces) {
		eat(TokenType::CLOSING_BRACE);
	} else {
		eat(TokenType::END);
	}

	return w;
}

ClassDeclaration* SyntaxicAnalyser::eatClassDeclaration() {

	ClassDeclaration* cd = new ClassDeclaration();

	eat(TokenType::CLASS);
	cd->name = eatIdent()->token->content;
	eat(TokenType::OPEN_BRACE);

	while (t->type == TokenType::LET) {
		cd->fields.push_back(eatVariableDeclaration());
	}

	eat(TokenType::CLOSING_BRACE);

	return cd;
}

Ident* SyntaxicAnalyser::eatIdent() {
	return new Ident(eat(TokenType::IDENT));
}

Token* SyntaxicAnalyser::eat() {
	return eat(TokenType::DONT_CARE);
}

Token* SyntaxicAnalyser::eat(TokenType type) {

	Token* eaten = t;

	lt = t;
	if (i < tokens.size() - 1) {
		t = &tokens.at(++i);
		// System.out.println(">> " + t.content);
	} else {
		t = new Token(TokenType::FINISHED, 0, 0, "");
		// System.out.println(">>>> done.");
	}
	nt = i < tokens.size() - 1 ? &tokens.at(i + 1) : nullptr;

	if (type != TokenType::DONT_CARE && eaten->type != type) {
		errors.push_back(new SyntaxicalError(eaten, "Expected token of type <" + to_string((int)type) + ">, got <" + to_string((int)eaten->type) + "> (" + eaten->content + ")"));
		return new Token(type, 0, 0, "**Error**");
	}
	return eaten;
}

Token* SyntaxicAnalyser::nextTokenAt(int pos) {
	if (i + pos < tokens.size())
		return &tokens.at(i + pos);
	else
		return nullptr;
}

vector<SyntaxicalError*> SyntaxicAnalyser::getErrors() {
	return errors;
}

long SyntaxicAnalyser::getTime() {
	return time;
}
