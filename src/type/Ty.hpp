#ifndef TY_HPP
#define TY_HPP

#include "LType.hpp"

namespace ls {

class Ty {
private:
	std::shared_ptr<LType> ptr;
public:
	Ty();
	Ty(Base_type* type);
	virtual ~Ty();

	std::shared_ptr<LType> operator->() { return ptr; }
	bool operator == (const Ty& ty) const;

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
	static Ty get_string();

	friend std::ostream& operator << (std::ostream&, const Ty&);
};

std::ostream& operator << (std::ostream&, const Ty&);

}

#endif