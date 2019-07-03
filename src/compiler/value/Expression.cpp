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
#include "../semantic/FunctionVersion.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

Expression::Expression(Value* v) : v1(v), operations(1) {}

void Expression::append(std::shared_ptr<Operator> op, Value* exp) {
	/*
	 * Single expression (2, 'hello', ...), just add the operator
	 */
	if (this->op == nullptr) {
		this->op = op;
		v2.reset(exp);
		return;
	}
	if (!parenthesis and (op->priority < this->op->priority or (op->priority == this->op->priority and op->character == "="))) {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a higher priority,
		 * such as : '× 7' => '5 + (2 × 7)'
		 */
		auto ex = new Expression();
		ex->v1 = std::move(v2);
		ex->op = op;
		ex->v2.reset(exp);
		this->v2.reset(ex);
	} else {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a lower priority,
		 * such as : '< 7' => '(5 + 2) < 7'
		 */
		auto newV1 = new Expression();
		newV1->v1 = std::move(this->v1);
		newV1->op = this->op;
		newV1->v2 = std::move(this->v2);
		this->v1.reset(newV1);
		this->op = op;
		v2.reset(exp);
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
	auto v1_location = v1->location();
	auto start = v1_location.start;
	auto end = op == nullptr ? v1->location().end : v2->location().end;
	return {v1_location.file, start, end};
}

void Expression::pre_analyze(SemanticAnalyzer* analyzer) {
	if (op == nullptr) {
		v1->pre_analyze(analyzer);
	} else {
		v1->pre_analyze(analyzer);
		v2->pre_analyze(analyzer);
		if (not analyzer->current_block()->is_loop and (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL)) {
			if (auto vv = dynamic_cast<VariableValue*>(v1.get())) {
				if (vv->var->scope != VarScope::CAPTURE) {
					// std::cout << "Pre-analyze update var " << vv->var << std::endl;
					previous_var = vv->var;
					vv->previous_var = vv->var;
					vv->var = analyzer->update_var(vv->var);
					variable = vv->var;
				}
			}
		}
	}
}

void Expression::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "Expression::analyze()" << std::endl;

	operations = 1;
	type = Type::any;

	// No operator : just analyze v1 and return
	if (op == nullptr) {
		v1->is_void = is_void;
		v1->analyze(analyzer);
		type = v1->type;
		throws = v1->throws;
		return;
	}

	v1->analyze(analyzer);
	v2->analyze(analyzer);

	throws = v1->throws or v2->throws;

	// in operator : v1 must be a container
	if (op->type == TokenType::IN and not v2->type->can_be_container()) {
		analyzer->add_error({Error::Type::VALUE_MUST_BE_A_CONTAINER, location(), v2->location(), {v2->to_string()}});
		return;
	}

	// A = B, A += B, etc. A must be a l-value
	if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::MINUS_EQUAL or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::BIT_AND_EQUALS or op->type == TokenType::BIT_OR_EQUALS or op->type == TokenType::BIT_XOR_EQUALS
		or op->type == TokenType::POWER_EQUAL or op->type == TokenType::INT_DIV_EQUAL) {
		// Change the type of x for operator =
		if (op->type == TokenType::EQUAL) {
			if (v2->type->is_void()) {
				analyzer->add_error({Error::Type::CANT_ASSIGN_VOID, location(), v2->location(), {v1->to_string()}});
			}
			// std::cout << "Expression change type " << v1->type << " = " << v2->type << std::endl;
		}
		// TODO other operators like |= ^= &=
		if (v1->type->constant) {
			analyzer->add_error({Error::Type::CANT_MODIFY_CONSTANT_VALUE, location(), op->token->location, {v1->to_string()}});
			return; // don't analyze more
		}
		// Check if A is a l-value
		if (not v1->isLeftValue()) {
			analyzer->add_error({Error::Type::VALUE_MUST_BE_A_LVALUE, location(), v1->location(), {v1->to_string()}});
			return; // don't analyze more
		}
		if (previous_var) {
			v1->type = previous_var->type;
		}
		// Change the type of x for operator =
		if (op->type == TokenType::EQUAL) {
			// std::cout << "Expression change type " << v1->type << " = " << v2->type << std::endl;
			if (variable) {
				// std::cout << "Expresssion update var type " << vv->var << " " << v2->type << std::endl;
				variable->type = v2->type->not_temporary();
				v1->type = variable->type;
			}
		}
	}

	// Merge operations count
	// (2 + 3) × 4    ->  2 ops for the × directly
	if (op->type != TokenType::OR or op->type == TokenType::AND) {
		if (Expression* ex1 = dynamic_cast<Expression*>(v1.get())) {
			operations += ex1->operations;
			ex1->operations = 0;
		}
		if (Expression* ex2 = dynamic_cast<Expression*>(v2.get())) {
			operations += ex2->operations;
			ex2->operations = 0;
		}
	}

	auto v1_type = op->reversed ? v2->type : v1->type;
	auto v2_type = op->reversed ? v1->type : v2->type;

	// std::cout << "Expression " << v1_type << " " << op->token->content << " " << v2_type << std::endl;

	if (v1_type->class_name().size()) {
		auto object_class = (LSClass*) analyzer->vm->internal_vars[v1_type->class_name()]->lsvalue;
		auto callable = object_class->getOperator(analyzer, op->character);
		if (callable) {
			// std::cout << "Callable : " << callable << std::endl;
			callable_version = callable->resolve(analyzer, {v1_type, v2_type});
			if (callable_version) {
				// std::cout << "Callable version : " << callable_version << std::endl;
				throws |= callable_version->flags & Module::THROWS;
				callable_version->apply_mutators(analyzer, {v1.get(), v2.get()});
				// For placeholder types, keep them no matter the operator
				auto return_type = callable_version->type->return_type();
				if (op->type == TokenType::PLUS or op->type == TokenType::MINUS or op->type == TokenType::TIMES or op->type == TokenType::MODULO) {
					if (v1->type->placeholder) { return_type = v1_type; }
					if (v2->type->placeholder) { return_type = v2_type; }
				}
				type = is_void ? Type::void_ : return_type;
				if ((v2_type->is_function() or v2_type->is_function_pointer() or v2_type->is_function_object()) and (callable_version->type->argument(1)->is_function() or callable_version->type->argument(1)->is_function_pointer())) {
					v2->will_take(analyzer, callable_version->type->argument(1)->arguments(), 1);
					v2->set_version(analyzer, callable_version->type->argument(1)->arguments(), 1);
				}
				// std::cout << "Operator " << v1->to_string() << " (" << v1->type << ") " << op->character << " " << v2->to_string() << "(" << v2->type << ") found! " << return_type << std::endl;
				return;
			}
		}
	}
	// std::cout << "No such operator " << v1->type << " " << op->character << " " << v2->type << std::endl;

	// Don't use old stuff for boolean, /, and bit operators
	if ((v1->type->is_bool() and op->type != TokenType::EQUAL) 
		or op->type == TokenType::TRIPLE_EQUAL
		or op->type == TokenType::PLUS or op->type == TokenType::MINUS
		or op->type == TokenType::TIMES or op->type == TokenType::DIVIDE 
		or op->type == TokenType::BIT_AND or op->type == TokenType::PIPE or op->type == TokenType::BIT_XOR
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS) {
		if (v1->type->placeholder or v2->type->placeholder) return;
		analyzer->add_error({Error::Type::NO_SUCH_OPERATOR, location(), op->token->location, {v1->type->to_string(), op->character, v2->type->to_string()}});
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
		auto vv = dynamic_cast<VariableValue*>(v1.get());
		if (op->type == TokenType::EQUAL and vv != nullptr) {
			if (is_void) {
				type = Type::void_;
			} else {
				type = v2->type;
			}
		} else {
			if (v1->type->is_any() || v2->type->is_any()) {
				type = Type::any;
			} else {
				type = v1->type;
			}
		}
		if (type->is_void() and not is_void) {
			type = v2_type;
		}
		// type.reference = false;
		type = type->add_temporary();
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = v1->type->operator * (v2->type);
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
		return c.insn_convert(c.new_integer(0), c.fun->getReturnType());
	}

	if (callable_version) {
		std::vector<Compiler::value> args;
		args.push_back([&](){ if (callable_version->v1_addr) {
			return ((LeftValue*) v1.get())->compile_l(c);
		} else {
			auto v = v1->compile(c);
			if (callable_version->symbol and v.t->is_primitive() and callable_version->type->argument(0)->is_any()) {
				v = c.insn_to_any(v);
			}
			return v;
		}}());
		args.push_back([&](){ if (callable_version->v2_addr) {
			return ((LeftValue*) v2.get())->compile_l(c);
		} else {
			auto v = v2->compile(c);
			if (callable_version->symbol and v.t->is_primitive() and callable_version->type->argument(1)->is_any()) {
				v = c.insn_to_any(v);
			}
			return v;
		}}());
		if (op->reversed) std::reverse(args.begin(), args.end());
		auto r = callable_version->compile_call(c, args, is_void);
		v1->compile_end(c);
		v2->compile_end(c);
		return r;
	}

	switch (op->type) {
		case TokenType::EQUAL: {
			// array[] = 12, array push
			auto array_access = dynamic_cast<ArrayAccess*>(v1.get());
			if (array_access && array_access->key == nullptr) {
				auto x_addr = ((LeftValue*) array_access->array.get())->compile_l(c);
				auto y = c.insn_to_any(v2->compile(c));
				auto r = c.insn_invoke(Type::any, {x_addr, y}, "Value.operator+=");
				v2->compile_end(c);
				return r;
			}
			// Normal a = b operator
			auto vv = dynamic_cast<VariableValue*>(v1.get());
			auto x_addr = !previous_var ? static_cast<LeftValue*>(v1.get())->compile_l(c) : Compiler::value();
			// Compile new value
			auto y = v2->compile(c);
			v2->compile_end(c);
			if (vv == nullptr) {
				y = c.insn_convert(y, v1->type);
			}
			// Move the object
			y = c.insn_move_inc(y);
			y.t = y.t->not_temporary();
			// Delete previous variable reference
			if (previous_var) {
				c.insn_delete_variable(previous_var->val);
			} else {
				c.insn_delete_variable(x_addr);
			}
			// Create the new variable
			if (vv != nullptr and previous_var) {
				// std::cout << "expression = entry" << std::endl;
				vv->var->create_entry(c);
				vv->var->store_value(c, y);
			} else {
				c.insn_store(x_addr, y);
			}
			if (is_void) {
				return {};
			} else {
				if (y.t->is_mpz_ptr()) {
					return c.insn_clone_mpz(y);
				} else {
					return y;
				}
			}
		}
		case TokenType::DOUBLE_QUESTION_MARK: {
			// x ?? y ==> if (x != null) { x } else { y }
			Compiler::label label_then = c.insn_init_label("then");
			Compiler::label label_else = c.insn_init_label("else");
			Compiler::label label_end = c.insn_init_label("end");

			auto x = c.insn_convert(v1->compile(c), type);
			v1->compile_end(c);
			auto condition = c.insn_call(Type::boolean, {x}, "Value.is_null");
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
			auto PN = c.builder.CreatePHI(type->llvm(c), 2);
			PN->addIncoming(y.v, label_then.block);
			PN->addIncoming(x.v, label_else.block);
			return {PN, type};
		}
		case TokenType::CATCH_ELSE: {
			auto r = c.create_entry("r", type);
			c.insn_try_catch([&]() {
				auto v1_value = v1->compile(c);
				if (v1_value.v) { // can be void
					c.insn_store(r, c.insn_convert(v1_value, type->fold()));
				}
				v1->compile_end(c);
			}, [&]() {
				auto y = v2->compile(c);
				c.insn_store(r, c.insn_convert(y, type->fold()));
				v2->compile_end(c);
			});
			return c.insn_load(r);
			break;
		}
		default: {
			std::cout << "No such operator to compile : " << op->character << " (" << (int) op->type << ")" << std::endl; // LCOV_EXCL_LINE
			assert(false); // LCOV_EXCL_LINE
		}
	}
}

std::unique_ptr<Value> Expression::clone() const {
	auto ex = std::make_unique<Expression>();
	ex->v1 = v1->clone();
	ex->op = op;
	ex->v2 = v2 ? v2->clone() : nullptr;
	return ex;
}

}
