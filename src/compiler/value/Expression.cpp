#include "Expression.hpp"
#include "ArrayAccess.hpp"
#include "ObjectAccess.hpp"
#include "Function.hpp"
#include "LeftValue.hpp"
#include "Number.hpp"
#include "VariableValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSBoolean.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/standard/BooleanSTD.hpp"
#include "../../vm/Exception.hpp"
#include "../../type/RawType.hpp"

using namespace std;

namespace ls {

Expression::Expression() : Expression(nullptr) {}

Expression::Expression(Value* v) :
	v1(v), v2(nullptr), op(nullptr), no_op(false), operations(1) {
}

Expression::~Expression() {
	if (v1 != nullptr) {
		delete v1;
	}
	if (v2 != nullptr) {
		delete v2;
	}
}

void Expression::append(std::shared_ptr<Operator> op, Value* exp) {
	/*
	 * Single expression (2, 'hello', ...), just add the operator
	 */
	if (this->op == nullptr) {
		this->op = op;
		v2 = exp;
		return;
	}
	if (!parenthesis and (op->priority < this->op->priority or (op->priority == this->op->priority and op->character == "="))) {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a higher priority,
		 * such as : '× 7' => '5 + (2 × 7)'
		 */
		auto ex = new Expression();
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
		auto newV1 = new Expression();
		newV1->v1 = this->v1;
		newV1->op = this->op;
		newV1->v2 = this->v2;
		this->v1 = newV1;
		this->op = op;
		v2 = exp;
	}
}

void Expression::print(std::ostream& os, int indent, bool debug, bool condensed) const {

	if (parenthesis or debug) {
		os << "(";
	}

	if (v1 != nullptr) {

		v1->print(os, indent, debug);

		if (op != nullptr) {
			os << " ";
			op->print(os);
			os << " ";
			v2->print(os, indent, debug);
		}
	}
	if (parenthesis or debug) {
		os << ")";
	}
	if (debug) {
		os << " " << type;
	}
}

Location Expression::location() const {
	auto start = v1->location().start;
	auto end = op == nullptr ? v1->location().end : v2->location().end;
	return {start, end};
}

void Expression::analyse(SemanticAnalyser* analyser) {

	operator_fun = nullptr;
	operations = 1;
	type = Type::ANY;

	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser);
		type = v1->type;
		return;
	}

	v1->analyse(analyser);
	v2->analyse(analyser);

	// in operator : v1 must be a container
	if (op->type == TokenType::IN and !v2->type.is_any() and !v2->type.is_container()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_CONTAINER, location(), v2->location(), {v2->to_string()}});
		return;
	}

	// A = B, A += B, etc. A must be a l-value
	if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::MINUS_EQUAL or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::BIT_AND_EQUALS or op->type == TokenType::BIT_OR_EQUALS or op->type == TokenType::BIT_XOR_EQUALS
		or op->type == TokenType::POWER_EQUAL or op->type == TokenType::INT_DIV_EQUAL) {
		// TODO other operators like |= ^= &=
		if (v1->type.constant) {
			analyser->add_error({SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, location(), op->token->location, {v1->to_string()}});
			return; // don't analyse more
		}
		// Check if A is a l-value
		if (not v1->isLeftValue()) {
			analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_LVALUE, location(), v1->location(), {v1->to_string()}});
			return; // don't analyse more
		}
		// Change the type of x for operator =
		if (op->type == TokenType::EQUAL) {
			if (v2->type._types.size() == 0) {
				analyser->add_error({SemanticError::Type::CANT_ASSIGN_VOID, location(), v2->location(), {v1->to_string()}});
			}
			if (v1->type.not_temporary() != v2->type.not_temporary()) {
				((LeftValue*) v1)->change_value(analyser, v2);
			}
		}
	}

	// Merge operations count
	// (2 + 3) × 4    ->  2 ops for the × directly
	if (op->type != TokenType::OR or op->type == TokenType::AND) {
		if (Expression* ex1 = dynamic_cast<Expression*>(v1)) {
			operations += ex1->operations;
			ex1->operations = 0;
		}
		if (Expression* ex2 = dynamic_cast<Expression*>(v2)) {
			operations += ex2->operations;
			ex2->operations = 0;
		}
	}

	this->v1_type = op->reversed ? v2->type : v1->type;
	this->v2_type = op->reversed ? v1->type : v2->type;

	auto object_class = [&]() { if (v1_type.clazz().size()) {
		return (LSClass*) analyser->vm->internal_vars.at(v1_type.clazz())->lsvalue;
	} else {
		return (LSClass*) analyser->vm->internal_vars.at("Value")->lsvalue;
	} }();
	const LSClass::Operator* m = object_class->getOperator(op->character, v1_type, v2_type);
	if (m != nullptr) {
		// std::cout << "Operator " << v1->to_string() << " (" << v1->type << ") " << op->character << " " << v2->to_string() << "(" << v2->type << ") found! " << (void*) m->addr << std::endl;
		operator_fun = m->addr;
		is_native_method = m->native;
		native_method_v1_addr = m->v1_addr;
		native_method_v2_addr = m->v2_addr;
		v1_type = op->reversed ? m->operand_type : m->object_type;
		v2_type = op->reversed ? m->object_type : m->operand_type;
		return_type = m->return_type;
		type = return_type;
		// Apply mutators
		for (const auto& mutator : m->mutators) {
			mutator->apply(analyser, {v1, v2});
		}
		return;
	} else {
		// std::cout << "No such operator " << v1->type << " " << op->character << " " << v2->type << std::endl;
	}

	// Don't use old stuff for boolean
	if ((v1->type == Type::BOOLEAN and op->type != TokenType::EQUAL) or op->type == TokenType::DIVIDE) {
		analyser->add_error({SemanticError::Type::NO_SUCH_OPERATOR, location(), op->token->location, {v1->type.to_string(), op->character, v2->type.to_string()}});
		return;
	}

	/*
	 * OLD
	 */
	if (v1->type.is_polymorphic() or v2->type.is_polymorphic()) {
		type = Type::ANY;
	}
	constant = v1->constant and v2->constant;

	// A = B, A += B, A * B, etc. mix types
	if (op->type == TokenType::EQUAL or op->type == TokenType::XOR
		or op->type == TokenType::PLUS or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::TIMES or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE or op->type == TokenType::DIVIDE_EQUAL
		or op->type == TokenType::MINUS or op->type == TokenType::MINUS_EQUAL
		or op->type == TokenType::POWER_EQUAL
		or op->type == TokenType::MODULO or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS
		or op->type == TokenType::GREATER or op->type == TokenType::GREATER_EQUALS
		or op->type == TokenType::SWAP
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS or op->type == TokenType::CATCH_ELSE or op->type == TokenType::DOUBLE_MODULO or op->type == TokenType::DOUBLE_MODULO_EQUALS
		) {

		auto vv = dynamic_cast<VariableValue*>(v1);

		// Set the correct type nature for the two members
		if (v2->type.is_polymorphic() and v1->type.is_primitive() and !(vv and op->type == TokenType::EQUAL)) {
			v1_type = Type::ANY;
		}
		if (v1->type.is_polymorphic() and v2->type.is_primitive() and !(vv and op->type == TokenType::EQUAL)) {
			v2_type = Type::ANY;
		}
		if (op->type == TokenType::EQUAL and vv != nullptr) {
			type = v2->type;
		} else {
			if (v1->type == Type::ANY || v2->type == Type::ANY) {
				type = Type::ANY;
			} else {
				type = v1->type;
			}
		}
		if (type == Type::MPZ) {
			type.temporary = true;
		}
		type.reference = false;
	}

	// Bitwise operators : result is a integer
	if (op->type == TokenType::BIT_AND or op->type == TokenType::PIPE or op->type == TokenType::BIT_XOR
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS) {

		type = Type::INTEGER;
	}

	// [1, 2, 3] ~~ x -> x ^ 2
	if (op->type == TokenType::TILDE_TILDE) {
		auto version = { v1->type.element() };
		v2->will_take(analyser, version, 1);
		v2->set_version(version, 1);
		type = Type::array(v2->version_type(version).return_type());
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = v1->type * v2->type;
		v1_type = type;
		v2_type = type;
	}
}

LSValue* jit_add(LSValue* x, LSValue* y) {
	return x->add(y);
}
LSValue* jit_sub(LSValue* x, LSValue* y) {
	return x->sub(y);
}
LSValue* jit_mul(LSValue* x, LSValue* y) {
	return x->mul(y);
}
LSValue* jit_div(LSValue* x, LSValue* y) {
	return x->div(y);
}
LSValue* jit_double_mod(LSValue* x, LSValue* y) {
	return x->double_mod(y);
}
int jit_bit_shl(LSValue* x, LSValue* y) {
	int r = (int) ((LSNumber*) x)->value << (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shl_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value << (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr(LSValue* x, LSValue* y) {
	int r = (int) ((LSNumber*) x)->value >> (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value >> (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr_unsigned(LSValue* x, LSValue* y) {
	int r = (uint32_t) ((LSNumber*) x)->value >> (uint32_t) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr_unsigned_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (uint32_t) ((LSNumber*) x)->value >> (uint32_t) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

Compiler::value Expression::compile(Compiler& c) const {

	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	c.mark_offset(location().start.line);

	// Increment operations
	if (operations > 0) {
		c.inc_ops(operations);
	}

	// Special case x / 0 : compile a throw exception instead
	if ((op->type == TokenType::DIVIDE or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::INT_DIV or op->type == TokenType::INT_DIV_EQUAL or op->type == TokenType::MODULO or op->type == TokenType::MODULO_EQUAL) and v2->is_zero()) {
		c.mark_offset(op->token->location.start.line);
		c.insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
		return {nullptr, {}};
	}

	if (operator_fun != nullptr) {

		vector<Compiler::value> args;
		// TODO simplify condition
		if ((v1->type.is_bool() and op->type == TokenType::EQUAL) or native_method_v1_addr) {
			args.push_back(((LeftValue*) v1)->compile_l(c));
			if (native_method_v2_addr) {
				args.push_back(((LeftValue*) v2)->compile_l(c));
			} else {
				args.push_back(v2->compile(c));
			}
		} else {
			args.push_back(op->reversed ? v2->compile(c) : v1->compile(c));
			args.push_back(op->reversed ? v1->compile(c) : v2->compile(c));
		}
		v1->compile_end(c);
		v2->compile_end(c);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) operator_fun;
			res = c.insn_call(type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, std::vector<Compiler::value>)) operator_fun;
			res = fun(c, args);
		}
		return res;
	}

	void* ls_func;
	vector<Compiler::value> args;
	Type ls_returned_type = type;

	switch (op->type) {
		case TokenType::EQUAL: {
			// array[] = 12, array push
			auto array_access = dynamic_cast<ArrayAccess*>(v1);
			if (array_access && array_access->key == nullptr) {
				auto x_addr = ((LeftValue*) array_access->array)->compile_l(c);
				auto y = c.insn_to_any(v2->compile(c));
				v2->compile_end(c);
				return c.insn_invoke(Type::ANY, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					return (*x)->add_eq(y);
				});
			}
			// Normal a = b operator
			auto vv = dynamic_cast<VariableValue*>(v1);
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			// Compile new value
			auto y = v2->compile(c);
			v2->compile_end(c);
			if (vv == nullptr) {
				y = c.insn_convert(y, v1->type);
			}
			// Move the object
			y = c.insn_move_inc(y);
			// Delete previous variable reference
			c.insn_delete(c.insn_load(x_addr));
			// Create the new variable
			if (vv != nullptr && vv->scope != VarScope::PARAMETER) {
				c.update_var(vv->name, y);
			} else {
				c.insn_store(x_addr, y);
			}
			return y;
		}
		case TokenType::PLUS_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			v1->compile_end(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::ANY, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->add_eq(y);
			});
		}
		case TokenType::MINUS_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::ANY, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->sub_eq(y);
			});
		}
		case TokenType::TIMES_EQUAL: {
			auto x = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::ANY, {x, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->mul_eq(y);
			});
		}
		case TokenType::DIVIDE_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::ANY, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->div_eq(y);
			});
		}
		case TokenType::MODULO_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::ANY, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->mod_eq(y);
			});
		}
		case TokenType::POWER_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::ANY, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->pow_eq(y);
			});
		}
		case TokenType::PLUS: {
			ls_func = (void*) &jit_add;
			break;
		}
		case TokenType::MINUS: {
			ls_func = (void*) &jit_sub;
			break;
		}
		case TokenType::TIMES: {
			ls_func = (void*) &jit_mul;
			break;
		}
		case TokenType::TILDE_TILDE: {
			if (v1->type.element() == Type::INTEGER) {
				if (type.element() == Type::INTEGER) {
					auto m = &LSArray<int>::ls_map<LSFunction*, int>;
					ls_func = (void*) m;
				} else if (type.element() == Type::REAL) {
					auto m = &LSArray<int>::ls_map<LSFunction*, double>;
					ls_func = (void*) m;
				} else {
					auto m = &LSArray<int>::ls_map<LSFunction*, LSValue*>;
					ls_func = (void*) m;
				}
			} else if (v1->type.element() == Type::REAL) {
				if (type.element() == Type::REAL) {
					auto m = &LSArray<double>::ls_map<LSFunction*, double>;
					ls_func = (void*) m;
				} else if (type.element() == Type::INTEGER) {
					auto m = &LSArray<double>::ls_map<LSFunction*, int>;
					ls_func = (void*) m;
				} else {
					// FIXME Ugly style to fix uncovered line
					auto m = &LSArray<double>::ls_map<LSFunction*, LSValue*>; ls_func = (void*) m;
				}
			} else {
				auto m = &LSArray<LSValue*>::ls_map<LSFunction*, LSValue*>;
				ls_func = (void*) m;
			}
			break;
		}
		case TokenType::BIT_SHIFT_LEFT : {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				return c.insn_shl(x, y);
			}
			ls_func = (void*) &jit_bit_shl;
			break;
		}
		case TokenType::BIT_SHIFT_LEFT_EQUALS : {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				auto a = c.insn_shl(c.insn_load(x_addr), y);
				c.insn_store(x_addr, a);
				return a;
			} else {
				ls_func = (void*) &jit_bit_shl_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT : {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				return c.insn_ashr(x, y);
			}
			ls_func = (void*) &jit_bit_shr;
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_EQUALS : {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				auto a = c.insn_lshr(c.insn_load(x_addr), y);
				c.insn_store(x_addr, a);
				return a;
			} else {
				ls_func = (void*) &jit_bit_shr_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_UNSIGNED : {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				return c.insn_lshr(x, y);
			}
			ls_func = (void*) &jit_bit_shr_unsigned;
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS : {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				auto a = c.insn_lshr(c.insn_load(x_addr), y);
				c.insn_store(x_addr, a);
				return a;
			} else {
				ls_func = (void*) &jit_bit_shr_unsigned_equal;
			}
			break;
		}
		case TokenType::DOUBLE_QUESTION_MARK: {
			// x ?? y ==> if (x != null) { x } else { y }
			Compiler::label label_then = c.insn_init_label("then");
			Compiler::label label_else = c.insn_init_label("else");
			Compiler::label label_end = c.insn_init_label("end");

			auto x = c.insn_convert(v1->compile(c), type);
			v1->compile_end(c);
			auto condition = c.insn_call(Type::BOOLEAN, {x}, +[](LSValue* v) {
				return v->type == LSValue::NULLL;
			});
			c.insn_if_new(condition, &label_then, &label_else);

			c.insn_label(&label_then);
			auto y = c.insn_convert(v2->compile(c), type);
			v2->compile_end(c);
			c.insn_branch(&label_end);
			label_then.block = Compiler::builder.GetInsertBlock();

			c.insn_label(&label_else);
			c.insn_branch(&label_end);
			label_else.block = Compiler::builder.GetInsertBlock();

			c.insn_label(&label_end);
			auto PN = Compiler::builder.CreatePHI(type.llvm_type(), 2);
			PN->addIncoming(y.v, label_then.block);
			PN->addIncoming(x.v, label_else.block);
			return {PN, type};
		}
		case TokenType::CATCH_ELSE: {

			Compiler::label try_start;
			Compiler::label try_end;
			Compiler::label exception_thrown;
			Compiler::label no_exception;

			c.insn_label(&try_start);
			// auto r = c.insn_create_value(type);
			auto v1_value = v1->compile(c);
			if (v1_value.v) {
				// c.insn_store(r, v1_value);
			}
			v1->compile_end(c);
			c.insn_label(&try_end);

			c.insn_branch(&no_exception);
			c.insn_label(&exception_thrown);

			c.add_catcher(try_start, try_end, exception_thrown);

			auto y = v2->compile(c);
			v2->compile_end(c);
			// c.insn_store(r, y);
			c.insn_label(&no_exception);
			// return r;
			break;
		}
		case TokenType::DOUBLE_MODULO: {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				return c.insn_mod(c.insn_add(c.insn_mod(x, y), y), y);
			} else {
				ls_func = (void*) &jit_double_mod;
			}
			break;
		}
		case TokenType::DOUBLE_MODULO_EQUALS: {
			if (v1->type.is_primitive() and v2->type.is_primitive()) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				auto x = c.insn_load(x_addr);
				auto r = c.insn_mod(c.insn_add(c.insn_mod(x, y), y), y);
				c.insn_store(x_addr, r);
				return r;
			} else {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = c.insn_to_any(v2->compile(c));
				v2->compile_end(c);
				return c.insn_invoke(type, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					LSValue* res = (*x)->double_mod_eq(y);
					return res;
				});
			}
			break;
		}
		default: {
			std::cout << "No such operator to compile : " << op->character << " (" << (int) op->type << ")" << std::endl; // LCOV_EXCL_LINE
			assert(false); // LCOV_EXCL_LINE
		}
	}
	if (args.size() == 0) {
		args.push_back(c.insn_to_any(v1->compile(c)));
		args.push_back(c.insn_to_any(v2->compile(c)));
		v1->compile_end(c);
		v2->compile_end(c);
	}
	return c.insn_invoke(ls_returned_type, args, ls_func);
}

Value* Expression::clone() const {
	auto ex = new Expression();
	ex->v1 = v1->clone();
	ex->op = op;
	ex->v2 = v2 ? v2->clone() : nullptr;
	return ex;
}

}
