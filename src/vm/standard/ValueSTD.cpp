#include "ValueSTD.hpp"
#include "JsonSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include "../../vm/VM.hpp"
#include "../../type/RawType.hpp"

namespace ls {

ValueSTD::ValueSTD() : Module("Value") {

	LSValue::ValueClass = clazz;

	/*
	 * Static attributes
	 */
	static_field("unknown", Type::ANY, ValueSTD::unknown);

	/*
	 * Attributes
	 */
	field("class", Type::CLASS, ValueSTD::attr_class);

	/*
	 * Operators
	 */
	operator_("is", {
		{Type::CONST_ANY, Type::CONST_CLASS, Type::BOOLEAN, (void*) &ValueSTD::op_instanceof}
	});
	operator_("==", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_equals}
	});
	operator_("!=", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_not_equals}
	});
	operator_("<", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_lt}
	});
	operator_("<=", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_le}
	});
	operator_(">", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_gt}
	});
	operator_(">=", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_ge}
	});
	operator_("and", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_and}
	});
	operator_("&&", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_and}
	});
	operator_("or", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_or}
	});
	operator_("||", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_or}
	});
	operator_("xor", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_xor}
	});
	operator_("&", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::INTEGER, (void*) &ValueSTD::op_bit_and}
	});
	operator_("|", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::INTEGER, (void*) &ValueSTD::op_bit_or}
	});
	operator_("^", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::INTEGER, (void*) &ValueSTD::op_bit_xor}
	});
	operator_("in", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::BOOLEAN, (void*) &ValueSTD::op_in}
	});
	operator_("<=>", {
		{Type::ANY, Type::ANY, Type::ANY, (void*) &ValueSTD::op_swap_ptr, {}, false, true, true},
		{Type::INTEGER, Type::INTEGER, Type::INTEGER, (void*) &ValueSTD::op_swap_val, {}, false, true, true},
	});
	operator_("**", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::ANY, (void*) &ValueSTD::op_pow}
	});
	operator_("\\", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::LONG, (void*) &ValueSTD::op_int_div}
	});
	operator_("\\=", {
		{Type::CONST_ANY, Type::CONST_ANY, Type::LONG, (void*) &ValueSTD::op_int_div_eq, {}, false, true}
	});
	operator_("%", {
		{Type::ANY, Type::ANY, Type::REAL, (void*) &ValueSTD::op_mod},
	});
	operator_("/", {
		{Type::CONST_NUMBER, Type::CONST_NUMBER, Type::REAL, (void*) &ValueSTD::op_div},
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::ANY, {Type::CONST_ANY}, (void*) &ValueSTD::copy}
	});
	method("string", {
		{Type::STRING, {Type::CONST_ANY}, (void*) &ValueSTD::to_string},
		{Type::STRING, {Type::CONST_LONG}, (void*) &ValueSTD::to_string}
	});
	method("json", {
		{Type::STRING, {Type::CONST_ANY}, (void*) &JsonSTD::encode}
	});
	method("typeID", {
		{Type::INTEGER, {Type::CONST_ANY}, (void*) &ValueSTD::typeID}
	});
}

/*
 * Static attributes
 */
Compiler::value ValueSTD::unknown(Compiler& c) {
	return c.new_pointer(LSNumber::get(floor(1 + ((double) rand() / RAND_MAX) * 100)), Type::ANY);
}

/*
 * Attributes
 */
Compiler::value ValueSTD::attr_class(Compiler& c, Compiler::value a) {
	auto clazz = c.insn_class_of(a);
	c.insn_delete_temporary(a);
	return clazz;
}

/*
 * Operators
 */
Compiler::value ValueSTD::op_instanceof(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_eq(c.insn_class_of(args[0]), args[1]);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_equals(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_eq(args[0], args[1]);
}

Compiler::value ValueSTD::op_not_equals(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_ne(args[0], args[1]);
}

Compiler::value ValueSTD::op_lt(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0 or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN, {c.insn_to_any(args[0]), c.insn_to_any(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a < *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_lt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_le(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0	or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN, {c.insn_to_any(args[0]), c.insn_to_any(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a <= *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_le(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_gt(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0	or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN, {c.insn_to_any(args[0]), c.insn_to_any(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a > *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_gt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_ge(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0 or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN, {c.insn_to_any(args[0]), c.insn_to_any(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a >= *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_ge(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_and(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_and(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_or(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_or(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_xor(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_to_bool(args[0]);
	auto b = c.insn_to_bool(args[1]);
	auto r = c.insn_or(
		c.insn_and(a, c.insn_not_bool(b)),
		c.insn_and(b, c.insn_not_bool(a))
	);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_bit_and(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_invoke(Type::INTEGER, {args[0], c.insn_to_any(args[1])}, +[](LSValue* x, LSValue* y) {
		LSNumber *a, *b;
		if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
		}
		auto res = (int) a->value & (int) b->value;
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return res;
	});
}

Compiler::value ValueSTD::op_bit_or(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_invoke(Type::INTEGER, {c.insn_to_any(args[0]), c.insn_to_any(args[1])}, +[](LSValue* x, LSValue* y) {
		LSNumber *a, *b;
		if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
			(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
		}
		auto res = (int) a->value | (int) b->value;
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return res;
	});
}

Compiler::value ValueSTD::op_bit_xor(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_invoke(Type::INTEGER, {args[0], c.insn_to_any(args[1])},
	+[](LSValue* x, LSValue* y) {
		LSNumber *a, *b;
		if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
			(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
		}
		auto res = (int) a->value ^ (int) b->value;
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return res;
	});
}

Compiler::value ValueSTD::op_in(Compiler& c, std::vector<Compiler::value> args) {
	if (args[1].t == Type::INTEGER) {
		return c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, int b) {
			return a->in_i(b);
		});
	} else {
		return c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, LSValue* b) {
			return a->in(b);
		});
	}
}

Compiler::value ValueSTD::op_swap_val(Compiler& c, std::vector<Compiler::value> args) {
	auto x_addr = args[0];
	auto y_addr = args[1];
	auto x = c.insn_load(x_addr);
	auto y = c.insn_load(y_addr);
	c.insn_store(x_addr, y);
	c.insn_store(y_addr, x);
	return y;
}

Compiler::value ValueSTD::op_swap_ptr(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::ANY, args, +[](LSValue** x, LSValue** y) {
		auto tmp = *x;
		*x = *y;
		*y = tmp;
		return *x;
	});
}


Compiler::value ValueSTD::copy(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.temporary) {
		return args[0];
	}
	return c.clone(args[0]);
}

Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t == Type::BOOLEAN) {
		return c.insn_call(Type::STRING, args, +[](bool b) {
			return new LSString(b ? "true" : "false");
		});
	}
	if (args[0].t.is_integer()) {
		return c.insn_call(Type::STRING, args, +[](int v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t.is_long()) {
		return c.insn_call(Type::STRING, args, +[](long v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t.is_mpz()) {
		auto s = c.insn_call(Type::STRING, args, +[](__mpz_struct v) {
			char buff[10000];
			mpz_get_str(buff, 10, &v);
			return new LSString(buff);
		});
		if (args[0].t.temporary) {
			c.insn_delete_mpz(args[0]);
		}
		return s;
	}
	if (args[0].t == Type::REAL) {
		return c.insn_call(Type::STRING, args, +[](double v) {
			return new LSString(LSNumber::print(v));
		});
	}
	// Default type : pointer
	return c.insn_call(Type::STRING, args, (void*) &LSValue::ls_json);
}

Compiler::value ValueSTD::typeID(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_typeof(args[0]);
}

Compiler::value ValueSTD::op_pow(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::ANY, {c.insn_to_any(args[0]), c.insn_to_any(args[1])}, +[](LSValue* x, LSValue* y) {
		return x->pow(y);
	});
}

Compiler::value ValueSTD::op_mod(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_mod(args[0], args[1]);
}

Compiler::value ValueSTD::op_div(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_div(args[0], args[1]);
}

Compiler::value ValueSTD::op_int_div(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_invoke(Type::LONG, {c.insn_to_any(args[0]), c.insn_to_any(args[1])}, +[](LSValue* x, LSValue* y) {
		auto res = x->int_div(y);
		long v = ((LSNumber*) res)->value;
		LSValue::delete_temporary(res);
		return v;
	});
}
Compiler::value ValueSTD::op_int_div_eq(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_invoke(Type::LONG, {args[0], c.insn_to_any(args[1])}, +[](LSValue** x, LSValue* y) {
		auto res = (*x)->int_div_eq(y);
		return (long) ((LSNumber*) res)->value;
	});
}

}
