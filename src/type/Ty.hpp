#ifndef TY_HPP
#define TY_HPP

#include <vector>
#include <map>
#include "Base_type.hpp"

namespace ls {

class Ty {
private:
	std::vector<std::shared_ptr<const Base_type>> _types;
	static int id_generator;
	static int primes[];
	static std::map<int, std::shared_ptr<Base_type>> type_map;
public:
	Ty();
	Ty(std::shared_ptr<Base_type> type);
	virtual ~Ty();

	bool operator == (const Ty& ty) const;
	void add(const std::shared_ptr<const Base_type> type);
	Ty merge(const Ty&) const;
	Ty fold() const;
	Ty get_compatible(const Ty&) const;
	bool all(std::function<bool(const Base_type* const)> fun) const;
	bool iterable() const;
	bool container() const;

	static void init_types();
	static int get_next_id();
	static Ty get_void();
	static Ty get_any();
	static Ty get_bool();
	static Ty get_number();
	static Ty get_integer();
	static Ty get_real();
	static Ty get_int();
	static Ty get_long();
	static Ty get_double();
	static Ty get_mpz();
	static Ty get_array();
	static Ty get_array(const Ty&);
	static Ty get_string();

	friend std::ostream& operator << (std::ostream&, const Ty&);
};

std::ostream& operator << (std::ostream&, const Ty&);

}

#endif