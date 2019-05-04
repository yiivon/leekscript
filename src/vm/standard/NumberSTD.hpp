#ifndef NUMBER_STD_HPP
#define NUMBER_STD_HPP

#include <math.h>
#include "../Module.hpp"

namespace ls {

class LSNumber;

class NumberSTD : public Module {
public:

	class OverflowException : public std::exception {
		virtual const char* what() const throw() {
			return "Number is too big! (10 000 bits max)";
		}
	};

	NumberSTD();

	/*
	 * Operators
	 */
	static Compiler::value eq_mpz_mpz(Compiler& c, std::vector<Compiler::value>);
	static Compiler::value eq_int_mpz(Compiler& c, std::vector<Compiler::value>);
	static Compiler::value eq_mpz_int(Compiler& c, std::vector<Compiler::value>);

	static Compiler::value add_real_real(Compiler& c, std::vector<Compiler::value>);
	static LSValue* add_int_ptr(int a, LSValue* b);
	static Compiler::value add_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value add_mpz_int(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value add_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, bool no_return);
	static Compiler::value add_eq_real(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value sub_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_mpz_int(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_real_real(Compiler& c, std::vector<Compiler::value>);

	static Compiler::value sub_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_eq_real(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value mul_real_real(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_int_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_mpz_int(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_int_string(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value mul_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_eq_real(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value div_val_val(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value div_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value div_eq_real(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value int_div_val_val(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value int_div_eq_val_val(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value pow_real_real(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value pow_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value pow_mpz_int(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value pow_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value pow_eq_real(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value lt(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value le(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value gt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value gt_int_mpz(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value ge(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value mod_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mod(Compiler& c, std::vector<Compiler::value>);

	static Compiler::value mod_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mod_eq_real(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value double_mod(Compiler& c, std::vector<Compiler::value>);
	static Compiler::value double_mod_eq(Compiler& c, std::vector<Compiler::value>);

	static Compiler::value bit_and(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_and_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_or(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_or_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_xor(Compiler&, std::vector<Compiler::value>);
	static Compiler::value bit_xor_eq(Compiler&, std::vector<Compiler::value>);

	/*
	 * Methods
	 */
	static Compiler::value _int(Compiler&, std::vector<Compiler::value>);
	static Compiler::value _long(Compiler&, std::vector<Compiler::value>);

	static Compiler::value abs(Compiler&, std::vector<Compiler::value>);

	static double acos_ptr(LSNumber* x);
	static Compiler::value acos_real(Compiler&, std::vector<Compiler::value>);

	static double asin_ptr(LSNumber* x);
	static Compiler::value asin_real(Compiler&, std::vector<Compiler::value>);

	static double atan_ptr(LSNumber* x);
	static Compiler::value atan_real(Compiler&, std::vector<Compiler::value>);

	static LSValue* atan2_ptr_ptr(LSNumber* x, LSNumber* y);
	static Compiler::value atan2(Compiler&, std::vector<Compiler::value>);

	static LSValue* char_ptr(LSNumber* x);
	static LSValue* char_real(double x);
	static LSValue* char_int(int x);

	static double exp_ptr(LSNumber* x);
	static Compiler::value exp_real(Compiler&, std::vector<Compiler::value>);

	static int floor_ptr(LSNumber* x);
	static Compiler::value floor_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value floor_int(Compiler&, std::vector<Compiler::value>);

	static int round_ptr(LSNumber* x);
	static Compiler::value round_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value round_int(Compiler&, std::vector<Compiler::value>);

	static int ceil_ptr(LSNumber* x);
	static Compiler::value ceil_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value ceil_int(Compiler&, std::vector<Compiler::value>);

	static LSValue* isInteger_ptr(LSNumber* x);
	static Compiler::value isInteger(Compiler&, std::vector<Compiler::value>);

	static LSValue* max_ptr_ptr(LSNumber* x, LSNumber* y);
	static double max_ptr_float(LSNumber* x, double y);
	static double max_ptr_int(LSNumber* x, int y);
	static double max_float_ptr(double x, LSNumber* y);
	static Compiler::value max_float_float(Compiler&, std::vector<Compiler::value>);
	static double max_int_ptr(int x, LSNumber* y);

	static double min_ptr_ptr(LSNumber* x, LSNumber* y);
	static double min_ptr_float(LSNumber* x, double y);
	static double min_ptr_int(LSNumber* x, int y);
	static double min_float_ptr(double x, LSNumber* y);
	static Compiler::value min_float_float(Compiler&, std::vector<Compiler::value>);
	static double min_int_ptr(int x, LSNumber* y);

	static LSValue* cos_ptr(LSNumber* x);
	static Compiler::value cos_real(Compiler&, std::vector<Compiler::value>);

	static double sin_ptr(LSNumber* x);
	static Compiler::value sin_real(Compiler&, std::vector<Compiler::value>);

	static double tan_ptr(LSNumber* x);
	static Compiler::value tan_real(Compiler&, std::vector<Compiler::value>);

	static double sqrt_ptr(LSNumber* x);
	static double sqrt_int(int x);
	static Compiler::value sqrt_mpz(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sqrt_real(Compiler&, std::vector<Compiler::value>);

	static LSValue* cbrt_ptr(LSNumber* x);
	static Compiler::value cbrt_real(Compiler&, std::vector<Compiler::value>);

	static Compiler::value pow_ptr(Compiler&, std::vector<Compiler::value>);
	static Compiler::value pow_int(Compiler&, std::vector<Compiler::value>);

	static Compiler::value is_prime(Compiler&, std::vector<Compiler::value>);
	template<typename T>
	static int is_prime_number(T n);
	static Compiler::value is_prime_int(Compiler&, std::vector<Compiler::value>);
	static Compiler::value is_prime_long(Compiler&, std::vector<Compiler::value>);

	static Compiler::value hypot_ptr_ptr(Compiler&, std::vector<Compiler::value>);
	static Compiler::value hypot_real_real(Compiler&, std::vector<Compiler::value>);

	static Compiler::value log_ptr(Compiler&, std::vector<Compiler::value>);
	static Compiler::value log_real(Compiler&, std::vector<Compiler::value>);

	static Compiler::value log10_ptr(Compiler&, std::vector<Compiler::value>);
	static Compiler::value log10_real(Compiler&, std::vector<Compiler::value>);

	static double rand01();
	static Compiler::value randFloat(Compiler& c, std::vector<Compiler::value> args);
	static int randInt(int min, int max);

	static Compiler::value signum(Compiler& c, std::vector<Compiler::value> args);

	static double toDegrees_ptr(LSNumber* x);
	static Compiler::value toDegrees(Compiler& c, std::vector<Compiler::value> args);
	static double toRadians_ptr(LSNumber* x);
	static Compiler::value toRadians(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value fold(Compiler& c, std::vector<Compiler::value> args);

	static LSValue* int_to_string(int);
	static LSValue* long_to_string(long);
	static LSValue* real_to_string(double);
};

}

#endif
