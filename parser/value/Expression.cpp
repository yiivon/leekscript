#include "../../vm/VM.hpp"
#include "Expression.hpp"
#include "VariableValue.hpp"
#include "LeftValue.hpp"
#include "Number.hpp"
#include "Function.hpp"

Expression::Expression() {
	v1 = nullptr;
	v2 = nullptr;
	op = nullptr;
	fast = false;
	ignorev2 = false;
	no_op = false;
}

Expression::Expression(Value* v) {
	v1 = v;
	v2 = nullptr;
	op = nullptr;
	fast = false;
	ignorev2 = false;
	no_op = false;
}

Expression::~Expression() {}

void Expression::append(Operator* op, Value* exp) {

	/*
	 * Single expression (2, 'toto', ...), just add the operator
	 */
	if (this->op == nullptr) {
		this->op = op;
		v2 = exp;
		return;
	}

	if (!parenthesis and (op->priority < this->op->priority
		   or (op->priority == this->op->priority and op->character == "="))) {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a higher priority,
		 * such as : '× 7' => '5 + (2 × 7)'
		 */
		Expression* ex = new Expression();
		ex->v1 = v2;
		ex->op = op;
		ex->v2 = exp;
		this->v2 = ex;

	} else {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a lower priority,
		 * such as : '< 7' => '(5 + 2) < 7'
		 */
		Expression* newV1 = new Expression();
		newV1->v1 = this->v1;
		newV1->op = this->op;
		newV1->v2 = this->v2;
		this->v1 = newV1;
		this->op = op;
		v2 = exp;
	}
}

void Expression::print(ostream& os) const {

	os << "Exp {";

	if (v1 != nullptr) {

		v1->print(os);

		if (op != nullptr) {
			os << " ";
			op->print(os);
			os << " ";
			v2->print(os);
		}
	}
	os <<  "}";
}

void Expression::analyse(SemanticAnalyser* analyser, const Type req_info) {

	type = Type::VALUE;
	constant = true;

	Type v1_type = Type::NEUTRAL;
	Type v2_type = Type::NEUTRAL;

	if (op != nullptr) {
		if (op->type == TokenType::DIVIDE) {
			type.raw_type = RawType::FLOAT;
			v1_type = Type::FLOAT;
			v2_type = Type::FLOAT;
		}
	}

	if (v1 != nullptr) {
		v1->analyse(analyser, v1_type);
		if (v1->type.nature == Nature::POINTER) type.nature = Nature::POINTER;
		if (v1->constant == false) constant = false;
	}
	if (v2 != nullptr) {
		v2->analyse(analyser, v2_type);
		if (v2->type.nature == Nature::POINTER) type.nature = Nature::POINTER;
		if (v2->constant == false) constant = false;
	}
	this->fast = fast;

	if (v1 != nullptr and v2 != nullptr) {

		if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS
			or op->type == TokenType::TIMES or op->type == TokenType::MINUS) {

			type = v1->type.mix(v2->type);
		}

		if (op->type == TokenType::TILDE_TILDE) {

			v2->will_take(analyser, 0, Type::POINTER);

			VariableValue* vv = dynamic_cast<VariableValue*>(v2);
			if (vv != nullptr and vv->var->value != nullptr) {

				vv->var->will_take(analyser, 0, Type::POINTER);
				v2->type.setReturnType(vv->var->type.getReturnType());
			}
			type = Type::ARRAY;
		}
	}
}

LSValue* jit_not(LSValue* x) {
	return x->operator ! ();
}
LSValue* jit_minus(LSValue* x) {
	return x->operator - ();
}
LSValue* jit_add(LSValue* x, LSValue* y) {
	return y->operator + (x);
}
LSValue* jit_sub(LSValue* x, LSValue* y) {
	return y->operator - (x);
}
LSValue* jit_mul(LSValue* x, LSValue* y) {
	return y->operator * (x);
}
LSValue* jit_div(LSValue* x, LSValue* y) {
	return y->operator / (x);
}
LSValue* jit_pow(LSValue* x, LSValue* y) {
	return y->poww(x);
}
LSValue* jit_mod(LSValue* x, LSValue* y) {
	return y->operator % (x);
}
LSValue* jit_and(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->isTrue() and y->isTrue());
}
LSValue* jit_or(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->isTrue() or y->isTrue());
}
LSValue* jit_inc(LSValue* x) {
	return x->operator ++ (1);
}
LSValue* jit_dec(LSValue* x) {
	return x->operator -- (1);
}
LSValue* jit_pre_inc(LSValue* x) {
	return x->operator ++ ();
}
LSValue* jit_pre_dec(LSValue* x) {
	return x->operator -- ();
}

LSValue* jit_equals(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->operator == (y));
}
LSValue* jit_not_equals(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->operator != (y));
}
LSValue* jit_lt(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator < (x));
}
LSValue* jit_le(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator <= (x));
}
LSValue* jit_gt(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator > (x));
}
LSValue* jit_ge(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator >= (x));
}

LSValue* jit_store(LSValue** x, LSValue* y) {
//	cout << "store" << endl;
	return *x = y;
}

LSValue* jit_swap(LSValue** x, LSValue** y) {
	LSValue* tmp = *x;
	*x = *y;
	*y = tmp;
	return *x;
}

LSValue* jit_add_equal(LSValue* x, LSValue* y) {
	return y->operator += (x);
}
LSValue* jit_sub_equal(LSValue* x, LSValue* y) {
	return y->operator -= (x);
}
LSValue* jit_mul_equal(LSValue* x, LSValue* y) {
	return y->operator *= (x);
}
LSValue* jit_div_equal(LSValue* x, LSValue* y) {
	return y->operator /= (x);
}
LSValue* jit_mod_equal(LSValue* x, LSValue* y) {
	return y->operator %= (x);
}
LSValue* jit_pow_equal(LSValue* x, LSValue* y) {
	return y->pow_eq(x);
}

LSArray* jit_tilde_tilde(LSArray* array, LSFunction* fun) {

	LSArray* new_array = new LSArray();

	typedef int (*FF)(LSValue*);
	FF f = (FF) fun->function;

	for (auto key : array->values) {
		new_array->push(LSNumber::get(f(key.second)));
	}
	return new_array;
}

LSArray* jit_tilde_tilde_pointer(LSArray* array, LSFunction* fun) {

	LSArray* new_array = new LSArray();

	typedef LSValue* (*FF)(LSValue*);
	FF f = (FF) fun->function;

	for (auto key : array->values) {
		new_array->push(f(key.second));
	}
	return new_array;
}

LSValue* jit_in(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->in(x));
}

jit_value_t Expression::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	if (op == nullptr) {
		return v1->compile_jit(c, F, req_type);
	}

	jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
	void* ls_func;
	bool use_jit_func = v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE;
	vector<jit_value_t> args;
	Type conv_info = req_type;
	Type v1_conv = Type::POINTER;
	Type v2_conv = Type::POINTER;

	switch (op->type) {
		case TokenType::EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_insn_store(F, x, y);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, y, req_type);
				}
				return y;
			} else if (v1->type.nature == Nature::POINTER) {
				if (dynamic_cast<VariableValue*>(v1)) {
					jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
					jit_value_t y = v2->compile_jit(c, F, Type::POINTER);
					jit_insn_store(F, x, y);
					return y;
				} else {
					args.push_back(((LeftValue*) v1)->compile_jit_l(c, F, Type::POINTER));
					args.push_back(v2->compile_jit(c, F, Type::POINTER));
					ls_func = (void*) &jit_store;
				}
			} else {
				cout << "!!!!!!!!!!" << endl;
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::POINTER);
				jit_insn_store(F, x, y);
				return y;
			}
			break;
		}
		case TokenType::SWAP: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t t = jit_insn_load(F, x);
				jit_insn_store(F, x, y);
				jit_insn_store(F, y, t);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, x, req_type);
				}
				return x;
			} else {
				args.push_back(((LeftValue*) v1)->compile_jit_l(c, F, Type::POINTER));
				args.push_back(((LeftValue*) v2)->compile_jit_l(c, F, Type::POINTER));
				ls_func = (void*) &jit_swap;
			}
			break;
		}
		case TokenType::PLUS_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t sum = jit_insn_add(F, x, y);
				jit_insn_store(F, x, sum);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_add_equal;
			}
			break;
		}
		case TokenType::MINUS_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t sum = jit_insn_sub(F, x, y);
				jit_insn_store(F, x, sum);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_sub_equal;
			}
			break;
		}
		case TokenType::TIMES_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t sum = jit_insn_mul(F, x, y);
				jit_insn_store(F, x, sum);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_mul_equal;
			}
			break;
		}
		case TokenType::DIVIDE_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t xf = jit_value_create(F, JIT_FLOAT);
				jit_insn_store(F, xf, x);
				jit_value_t sum = jit_insn_div(F, xf, y);
				jit_insn_store(F, x, sum);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_div_equal;
			}
			break;
		}
		case TokenType::MODULO_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t sum = jit_insn_rem(F, x, y);
				jit_insn_store(F, x, sum);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_mod_equal;
			}
			break;
		}
		case TokenType::POWER_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t sum = jit_insn_pow(F, x, y);
				jit_insn_store(F, x, sum);
				if (v2->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_pow_equal;
			}
			break;
		}
		case TokenType::AND: {
			jit_func = &jit_insn_and;
			ls_func = (void*) &jit_and;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::OR: {
			jit_func = &jit_insn_or;
			ls_func = (void*) &jit_or;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::PLUS: {
			jit_func = &jit_insn_add;
			ls_func = (void*) &jit_add;
			break;
		}
		case TokenType::MINUS: {
			jit_func = &jit_insn_sub;
			ls_func = (void*) &jit_sub;
			break;
		}
		case TokenType::TIMES: {
			jit_func = &jit_insn_mul;
			ls_func = (void*) &jit_mul;
			break;
		}
		case TokenType::DIVIDE: {
			jit_func = &jit_insn_div;
			ls_func = (void*) &jit_div;
			conv_info = Type::FLOAT;
			break;
		}
		case TokenType::MODULO: {
			jit_func = &jit_insn_rem;
			ls_func = (void*) &jit_mod;
			break;
		}
		case TokenType::POWER: {
			if (req_type.nature == Nature::POINTER) {
				use_jit_func = false;
			}
			jit_func = &jit_insn_pow;
			ls_func = (void*) &jit_pow;
			break;
		}
		case TokenType::DOUBLE_EQUAL: {
			jit_func = &jit_insn_eq;
			ls_func = (void*) &jit_equals;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::DIFFERENT: {
			jit_func = &jit_insn_ne;
			ls_func = (void*) &jit_not_equals;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::LOWER: {
			jit_func = &jit_insn_lt;
			ls_func = (void*) &jit_lt;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::LOWER_EQUALS: {
			jit_func = &jit_insn_le;
			ls_func = (void*) &jit_le;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::GREATER: {
			jit_func = &jit_insn_gt;
			ls_func = (void*) &jit_gt;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::GREATER_EQUALS: {
			jit_func = &jit_insn_ge;
			ls_func = (void*) &jit_ge;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::TILDE_TILDE: {
			if (v2->type.getReturnType().nature == Nature::VALUE) {
				ls_func = (void*) &jit_tilde_tilde;
			} else {
				ls_func = (void*) &jit_tilde_tilde_pointer;
			}
			v2_conv = Type(RawType::FUNCTION, Nature::POINTER);
			break;
		}
		case TokenType::IN: {
			use_jit_func = false;
			ls_func = (void*) &jit_in;
			break;
		}
		default: {
			throw new exception();
		}
		}

	if (use_jit_func) {

		jit_value_t x = v1->compile_jit(c, F, Type::NEUTRAL);
		jit_value_t y = v2->compile_jit(c, F, Type::NEUTRAL);
		/*
		jit_value_t is_int = jit_insn_eq(F,
			jit_insn_and(F, x, jit_value_create_nint_constant(F, jit_type_int, 2147483648)),
			jit_value_create_nint_constant(F, jit_type_int, 0)
		);*/
/*
		jit_value_t is_int = jit_insn_eq(F,
			x,
			jit_value_create_nint_constant(F, jit_type_int, -55)
		);

		jit_label_t label_end = jit_label_undefined;
*/
		jit_value_t r = jit_func(F, x, y);
/*
		jit_insn_branch_if_not(F, is_int, &label_end);

		jit_insn_store(F, x, y);

		jit_insn_label(F, &label_end);
*/
		if (req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, r, conv_info);
		}
		return r;

	} else {

		jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

		if (args.size() == 0) {
			args.push_back(v1->compile_jit(c, F, v1_conv));
			args.push_back(v2->compile_jit(c, F, v2_conv));
		}
		jit_value_t v = jit_insn_call_native(F, "", ls_func, sig, args.data(), 2, JIT_CALL_NOTHROW);
		if (v1->type.nature == Nature::VALUE and op->type == TokenType::PLUS_EQUAL) {
			jit_insn_store(F, args[0], v);
		}
		return v;
	}
}

