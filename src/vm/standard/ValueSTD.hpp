#ifndef VALUE_STD_HPP
#define VALUE_STD_HPP

#include "../Module.hpp"

namespace ls {

class ValueSTD : public Module {
public:
	ValueSTD();

	/*
	 * Static fields
	 */
	static Compiler::value unknown(Compiler& c);

	/*
	 * Fields
	 */
	static Compiler::value attr_class(Compiler& c, Compiler::value a);

	/*
	 * Operators
	 */
	static Compiler::value op_instanceof(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_equals(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_not_equals(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_lt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_le(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_gt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_ge(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_and(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_or(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_xor(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_add(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_sub(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_mul(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_div(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_pow(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_int_div(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_int_div_eq(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_mod(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_double_mod(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_double_mod_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_and(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_and_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_or(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_or_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_xor(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_xor_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_shift_left(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_shift_left_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_shift_right(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_shift_right_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_shift_uright(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_shift_uright_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_in(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_swap_val(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_swap_ptr(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_call(Compiler&, std::vector<Compiler::value>);

	/*
	 * Methods
	 */
	static Compiler::value copy(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value to_string(Compiler& c, std::vector<Compiler::value> args);

	// Hidden functions
	static Compiler::value typeID(Compiler& c, std::vector<Compiler::value> args);
	static int absolute(LSValue* v);
	static LSValue* clone(LSValue* v);
	static LSValue* attr(LSValue* v, char* field);
	static bool ls_not(LSValue* x);
	static LSValue* ls_minus(LSValue* x);
	static LSValue* ls_pre_inc(LSValue* x);
	static LSValue* ls_pre_dec(LSValue* x);
	static LSValue* ls_pre_tilde(LSValue* v);
};

}

#endif
