#include "PrefixExpression.hpp"
#include "FunctionCall.hpp"
#include "LeftValue.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

using namespace std;

namespace ls {

PrefixExpression::PrefixExpression() {
	expression = nullptr;
	operatorr = nullptr;
}

PrefixExpression::~PrefixExpression() {
	delete expression;
}

void PrefixExpression::print(ostream& os, int indent, bool debug) const {
	operatorr->print(os);
	if (operatorr->type == TokenType::NEW) {
		os << " ";
	}
	expression->print(os, indent, debug);
	if (debug) {
		os << " " << type;
	}
}

Location PrefixExpression::location() const {
	return {operatorr->token->location.start, expression->location().end};
}

void PrefixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	expression->analyse(analyser, Type::UNKNOWN);

	if (operatorr->type == TokenType::TILDE) {
		type = expression->type;
		if (type == Type::BOOLEAN) {
			type = Type::INTEGER;
		}
	}

	if (operatorr->type == TokenType::PLUS_PLUS
		or operatorr->type == TokenType::MINUS_MINUS
		or operatorr->type == TokenType::MINUS) {

		type = expression->type;
		if (type == Type::MPZ and operatorr->type == TokenType::MINUS) {
			type = Type::MPZ_TMP;
		}
		if (operatorr->type == TokenType::MINUS && req_type == Type::REAL) {
			type = Type::REAL;
		}
		if (operatorr->type == TokenType::PLUS_PLUS or operatorr->type == TokenType::MINUS_MINUS) {
			if (expression->type.constant) {
				analyser->add_error({SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, location(), expression->location(), {expression->to_string()}});
			}
		}

	} else if (operatorr->type == TokenType::NOT) {

		type = Type::BOOLEAN;

	} else if (operatorr->type == TokenType::NEW) {

		type = Type::POINTER;
		if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {
			if (vv->name == "Number") type = Type::INTEGER;
			else if (vv->name == "Boolean") type = Type::BOOLEAN;
			else if (vv->name == "String") type = Type::STRING;
			else if (vv->name == "Array") type = Type::PTR_ARRAY;
			else if (vv->name == "Object") type = Type::OBJECT;
			else if (vv->name == "Set") type = Type::PTR_SET;
		}
		else if (FunctionCall* fc = dynamic_cast<FunctionCall*>(expression)) {
			if (VariableValue* vv = dynamic_cast<VariableValue*>(fc->function)) {
				if (vv->name == "Number") {
					if (fc->arguments.size() > 0) {
						fc->arguments[0]->analyse(analyser, Type::UNKNOWN);
						type = fc->arguments[0]->type;
					} else {
						type = Type::INTEGER;
					}
				}
				else if (vv->name == "Boolean") type = Type::BOOLEAN;
				else if (vv->name == "String") type = Type::STRING;
				else if (vv->name == "Array") type = Type::PTR_ARRAY;
				else if (vv->name == "Object") type = Type::OBJECT;
				else if (vv->name == "Set") type = Type::PTR_SET;
			}
		}
	}
	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

bool jit_not(LSValue* x) {
	auto r = x->ls_not();
	LSValue::delete_temporary(x);
	return r;
}
LSValue* jit_minus(LSValue* x) {
	return x->ls_minus();
}
LSValue* jit_pre_inc(LSValue* x) {
	return x->ls_preinc();
}
LSValue* jit_pre_dec(LSValue* x) {
	return x->ls_predec();
}
LSValue* jit_pre_tilde(LSValue* v) {
	return v->ls_tilde();
}

Compiler::value PrefixExpression::compile(Compiler& c) const {

	c.mark_offset(location().start.line);

	Compiler::value arg;
	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type == Type::MPZ) {
				auto x = ((LeftValue*) expression)->compile_l(c);
				auto one = c.new_integer(1);
				c.insn_call(Type::VOID, {x, x, one}, &mpz_add_ui);
				return c.insn_load(x, 0, Type::MPZ);
			} else if (expression->type.nature == Nature::VALUE) {
				auto x_addr = ((LeftValue*) expression)->compile_l(c);
				auto x = c.insn_load(x_addr, 0, Type::INTEGER);
				auto sum = c.insn_add(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				if (type.nature == Nature::POINTER) {
					return c.insn_to_pointer(sum);
				}
				return sum;
			} else {
				arg = expression->compile(c);
				func = (void*) jit_pre_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				auto x_addr = ((LeftValue*) expression)->compile_l(c);
				auto x = c.insn_load(x_addr, 0, Type::INTEGER);
				auto sum = c.insn_sub(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				if (type.nature == Nature::POINTER) {
					return c.insn_to_pointer(sum);
				}
				return sum;
			} else {
				arg = expression->compile(c);
				func = (void*) jit_pre_dec;
			}
			break;
		}
		case TokenType::NOT: {
			if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				auto r = c.insn_not_bool(x);
				if (type.nature == Nature::POINTER) {
					return c.insn_to_pointer(r);
				}
				return r;
			} else {
				arg = expression->compile(c);
				func = (void*) jit_not;
			}
			break;
		}
		case TokenType::MINUS: {
			if (expression->type.not_temporary() == Type::MPZ) {
				auto x = expression->compile(c);
				return c.insn_call(Type::MPZ, {x}, +[](__mpz_struct x) {
					mpz_t neg;
					mpz_init(neg);
					mpz_neg(neg, &x);
					return *neg;
				});
			} else if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				auto r = c.insn_neg(x);
				if (type.nature == Nature::POINTER) {
					return c.insn_to_pointer(r);
				} else if (type == Type::REAL && expression->type == Type::INTEGER) {
					return c.to_real(r);
				}
				return r;
			} else {
				arg = expression->compile(c);
				func = (void*) jit_minus;
			}
			break;
		}
		case TokenType::TILDE: {
			if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				auto r = c.insn_not(x);
				if (type.nature == Nature::POINTER) {
					return c.insn_to_pointer(r);
				}
				return r;
			} else {
				arg = expression->compile(c);
				func = (void*) jit_pre_tilde;
			}
			break;
		}
		case TokenType::NEW: {

			if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {
				if (vv->name == "Number") {
					auto n = c.new_integer(0);
					if (type.nature == Nature::POINTER) {
						return c.insn_to_pointer(n);
					}
					return n;
				}
				else if (vv->name == "Boolean") {
					auto n = c.new_bool(0);
					if (type.nature == Nature::POINTER) {
						return c.insn_to_pointer(n);
					}
					return n;
				}
				else if (vv->name == "String") {
					return {c.new_pointer(new LSString("")).v, type};
				}
				else if (vv->name == "Array") {
					return {c.new_pointer(new LSArray<LSValue*>()).v, type};
				}
				else if (vv->name == "Object") {
					return {c.new_pointer(new LSObject()).v, type};
				}
				else if (vv->name == "Set") {
					return {c.new_pointer(new LSSet<LSValue*>()).v, type};
				}
			}
			if (FunctionCall* fc = dynamic_cast<FunctionCall*>(expression)) {
				if (VariableValue* vv = dynamic_cast<VariableValue*>(fc->function)) {
					if (vv->name == "Number") {
						if (fc->arguments.size() > 0) {
							auto n = fc->arguments[0]->compile(c);
							if (type.nature == Nature::POINTER) {
								return c.insn_to_pointer(n);
							}
							return n;
						} else {
							auto n = c.new_integer(0);
							if (type.nature == Nature::POINTER) {
								return c.insn_to_pointer(n);
							}
							return n;
						}
					}
					if (vv->name == "Boolean") {
						auto n = c.new_bool(false);
						if (type.nature == Nature::POINTER) {
							return c.insn_to_pointer(n);
						}
						return n;
					}
					if (vv->name == "String") {
						if (fc->arguments.size() > 0) {
							return fc->arguments[0]->compile(c);
						}
						return {c.new_pointer(new LSString("")).v, type};
					}
					if (vv->name == "Array") {
						return {c.new_pointer(new LSArray<LSValue*>()).v, type};
					}
					if (vv->name == "Object") {
						return {c.new_pointer(new LSObject()).v, type};
					}
					if (vv->name == "Set") {
						return {c.new_pointer(new LSSet<LSValue*>()).v, type};
					}
				}
				// new A(), convert to new A
				auto clazz = fc->function->compile(c);
				return c.new_object_class(clazz);
			}
			// By default, compile the class expression and construct a LSObject.
			// new A
			auto clazz = expression->compile(c);
			return c.new_object_class(clazz);
		}
		default: {}
	}
	return c.insn_call(type, {arg}, func);
}

Value* PrefixExpression::clone() const {
	auto pe = new PrefixExpression();
	pe->expression = (LeftValue*) expression->clone();
	pe->operatorr = operatorr;
	return pe;
}

}
