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
};

}

#endif