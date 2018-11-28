#ifndef TY_HPP
#define TY_HPP

#include <vector>
#include <map>
#include "Base_type.hpp"

namespace ls {

class Ty {
private:
	std::vector<std::shared_ptr<const Base_type>> _types;
	bool constant;

	static int id_generator;
	static int primes[];
	static std::map<int, std::shared_ptr<Base_type>> type_map;
public:
	Ty();
	Ty(std::shared_ptr<const Base_type> type, bool constant = false);
	virtual ~Ty();

	bool operator == (const Ty& ty) const;
	void add(const Ty type);
	void add(const std::shared_ptr<const Base_type> type);
	Ty merge(const Ty&) const;
	Ty fold() const;
	Ty get_compatible(const Ty&) const;
	bool all(std::function<bool(const Base_type* const)> fun) const;
	bool iterable() const;
	bool container() const;
	Ty element() const;
	Ty key() const;
	std::vector<Ty> arguments() const;
	Ty argument(int) const;
	Ty get_return() const;

	static void init_types();
	static int get_next_id();
	static Ty get_void();
	static Ty any();
	static Ty get_bool();
	static Ty const_bool();
	static Ty get_number();
	static Ty get_integer();
	static Ty real();
	static Ty get_int();
	static Ty get_long();
	static Ty get_double();
	static Ty mpz();
	static Ty array();
	static Ty array(const Ty&);
	static Ty get_string();
	static Ty get_class();
	static Ty object();
	static Ty interval();
	static Ty fun();
	static Ty get_char();

	static std::shared_ptr<Base_type> any_type;
	static std::shared_ptr<Base_type> bool_type;
	static std::shared_ptr<Base_type> int_type;
	static std::shared_ptr<Base_type> integer_type;
	static std::shared_ptr<Base_type> real_type;
	static std::shared_ptr<Base_type> double_type;
	static std::shared_ptr<Base_type> number_type;
	static std::shared_ptr<Base_type> long_type;
	static std::shared_ptr<Base_type> mpz_type;
	static std::shared_ptr<Base_type> string_type;
	static std::shared_ptr<Base_type> class_type;
	static std::shared_ptr<Base_type> object_type;
	static std::shared_ptr<Base_type> interval_type;
	static std::shared_ptr<Base_type> fun_type;
	static std::shared_ptr<Base_type> char_type;

	friend std::ostream& operator << (std::ostream&, const Ty&);
};

std::ostream& operator << (std::ostream&, const Ty&);

}

#endif