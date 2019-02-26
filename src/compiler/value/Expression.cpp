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
#include "../semantic/Callable.hpp"

namespace ls {

Expression::Expression() : Expression(nullptr) {}

Expression::Expression(Value* v) :
	v1(v), v2(nullptr), op(nullptr), operations(1) {
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
	// std::cout << "Expression::analyse()" << std::endl;

	operations = 1;
	type = Type::any();

	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser);
		type = v1->type;
		return;
	}

	v1->analyse(analyser);
	v2->analyse(analyser);

	// in operator : v1 must be a container
	if (op->type == TokenType::IN and not v2->type.can_be_container()) {
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

	auto v1_type = op->reversed ? v2->type : v1->type;
	auto v2_type = op->reversed ? v1->type : v2->type;

	const Callable* callable = nullptr;
	if (v1_type.class_name().size()) {
		auto object_class = (LSClass*) analyser->vm->internal_vars.at(v1_type.class_name())->lsvalue;
		callable = object_class->getOperator(analyser, op->character, v1_type, v2_type);
	}
	if (callable == nullptr) {
		auto v_class = (LSClass*) analyser->vm->internal_vars.at("Value")->lsvalue;
		callable = v_class->getOperator(analyser, op->character, v1_type, v2_type);
	}
	if (callable) {
		// std::cout << "Callable : " << callable << std::endl;
		callable_version = callable->resolve(analyser, {v1_type, v2_type});
		if (callable_version) {
			// std::cout << "Callable version : " << callable_version << std::endl;
			callable_version->apply_mutators(analyser, {v1, v2});
			// For placeholder types, keep them no matter the operator
			auto return_type = callable_version->type.return_type();
			if (op->type == TokenType::PLUS or op->type == TokenType::MINUS or op->type == TokenType::TIMES or op->type == TokenType::MODULO) {
				if (v1->type.is_placeholder()) { return_type = v1_type; }
				if (v2->type.is_placeholder()) { return_type = v2_type; }
			}
			type = return_type;
			if (v2_type.is_function()) {
				v2->will_take(analyser, callable_version->type.argument(1).arguments(), 1);
				v2->set_version(callable_version->type.argument(1).arguments(), 1);
				v2->must_return(analyser, callable_version->type.argument(1).return_type());
			}
			// std::cout << "Operator " << v1->to_string() << " (" << v1->type << ") " << op->character << " " << v2->to_string() << "(" << v2->type << ") found! " << return_type << std::endl;
			return;
		}
	}
	// std::cout << "No such operator " << v1->type << " " << op->character << " " << v2->type << std::endl;

	// Don't use old stuff for boolean, /, and bit operators
	if ((v1->type == Type::boolean() and op->type != TokenType::EQUAL) 
		or op->type == TokenType::PLUS or op->type == TokenType::MINUS 
		or op->type == TokenType::TIMES or op->type == TokenType::DIVIDE 
		or op->type == TokenType::BIT_AND or op->type == TokenType::PIPE or op->type == TokenType::BIT_XOR
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS) {
		analyser->add_error({SemanticError::Type::NO_SUCH_OPERATOR, location(), op->token->location, {v1->type.to_string(), op->character, v2->type.to_string()}});
		return;
	}

	/*
	 * OLD
	 */
	// A = B, A += B, A * B, etc. mix types
	if (op->type == TokenType::EQUAL or op->type == TokenType::XOR
		or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL
		or op->type == TokenType::MINUS_EQUAL
		or op->type == TokenType::POWER_EQUAL
		or op->type == TokenType::MODULO or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS
		or op->type == TokenType::GREATER or op->type == TokenType::GREATER_EQUALS
		or op->type == TokenType::SWAP or op->type == TokenType::CATCH_ELSE
		) {
		// Set the correct type nature for the two members
		auto vv = dynamic_cast<VariableValue*>(v1);
		if (op->type == TokenType::EQUAL and vv != nullptr) {
			if (is_void) {
				type = {};
			} else {
				type = v2->type;
			}
		} else {
			if (v1->type == Type::any() || v2->type == Type::any()) {
				type = Type::any();
			} else {
				type = v1->type;
			}
		}
		if (type.is_void() and not is_void) {
			type = v2_type;
		}
		type.reference = false;
		type.temporary = true;
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = v1->type * v2->type;
	}
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
		return {};
	}

	if (callable_version) {
		std::vector<Compiler::value> args;
		args.push_back([&](){ if (callable_version->v1_addr) {
			return ((LeftValue*) v1)->compile_l(c);
		} else {
			return v1->compile(c);
		}}());
		args.push_back([&](){ if (callable_version->v2_addr) {
			return ((LeftValue*) v2)->compile_l(c);
		} else {
			return v2->compile(c);
		}}());
		if (op->reversed) std::reverse(args.begin(), args.end());
		v1->compile_end(c);
		v2->compile_end(c);
		return callable_version->compile_call(c, args);
	}

	void* ls_func;
	std::vector<Compiler::value> args;

	switch (op->type) {
		case TokenType::EQUAL: {
			// array[] = 12, array push
			auto array_access = dynamic_cast<ArrayAccess*>(v1);
			if (array_access && array_access->key == nullptr) {
				auto x_addr = ((LeftValue*) array_access->array)->compile_l(c);
				auto y = c.insn_to_any(v2->compile(c));
				v2->compile_end(c);
				return c.insn_invoke(Type::any(), {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
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
			y.t = y.t.not_temporary();
			// Delete previous variable reference
			c.insn_delete(c.insn_load(x_addr));
			// Create the new variable
			if (vv != nullptr && vv->scope != VarScope::PARAMETER) {
				c.update_var(vv->name, y);
			} else {
				c.insn_store(x_addr, y);
			}
			if (is_void) {
				return {};
			} else {
				return y;
			}
		}
		case TokenType::PLUS_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			v1->compile_end(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::any(), {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->add_eq(y);
			});
		}
		case TokenType::MINUS_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::any(), {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->sub_eq(y);
			});
		}
		case TokenType::TIMES_EQUAL: {
			auto x = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::any(), {x, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->mul_eq(y);
			});
		}
		case TokenType::DIVIDE_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::any(), {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->div_eq(y);
			});
		}
		case TokenType::MODULO_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::any(), {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->mod_eq(y);
			});
		}
		case TokenType::POWER_EQUAL: {
			auto x_addr = ((LeftValue*) v1)->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			v2->compile_end(c);
			return c.insn_invoke(Type::any(), {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
				return (*x)->pow_eq(y);
			});
		}
		case TokenType::DOUBLE_QUESTION_MARK: {
			// x ?? y ==> if (x != null) { x } else { y }
			Compiler::label label_then = c.insn_init_label("then");
			Compiler::label label_else = c.insn_init_label("else");
			Compiler::label label_end = c.insn_init_label("end");

			auto x = c.insn_convert(v1->compile(c), type);
			v1->compile_end(c);
			auto condition = c.insn_call(Type::boolean(), {x}, +[](LSValue* v) {
				return v->type == LSValue::NULLL;
			});
			c.insn_if_new(condition, &label_then, &label_else);

			c.insn_label(&label_then);
			auto y = c.insn_convert(v2->compile(c), type);
			v2->compile_end(c);
			c.insn_branch(&label_end);
			label_then.block = c.builder.GetInsertBlock();

			c.insn_label(&label_else);
			c.insn_branch(&label_end);
			label_else.block = c.builder.GetInsertBlock();

			c.insn_label(&label_end);
			auto PN = c.builder.CreatePHI(type.llvm_type(c), 2);
			PN->addIncoming(y.v, label_then.block);
			PN->addIncoming(x.v, label_else.block);
			return {PN, type};
		}
		case TokenType::CATCH_ELSE: {
			auto r = c.create_entry("r", type);
			c.insn_try_catch([&]() {
				auto v1_value = v1->compile(c);
				if (v1_value.v) { // can be void
					c.insn_store(r, c.insn_convert(v1_value, type.fold()));
				}
				v1->compile_end(c);
			}, [&]() {
				auto y = v2->compile(c);
				v2->compile_end(c);
				c.insn_store(r, c.insn_convert(y, type.fold()));
			});
			return c.insn_load(r);
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
	return c.insn_invoke(type, args, ls_func);
}

Value* Expression::clone() const {
	auto ex = new Expression();
	ex->v1 = v1->clone();
	ex->op = op;
	ex->v2 = v2 ? v2->clone() : nullptr;
	return ex;
}

}
