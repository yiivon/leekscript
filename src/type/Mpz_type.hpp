#ifndef MPZ_TYPE_HPP
#define MPZ_TYPE_HPP

#include "Integer_type.hpp"

namespace ls {

class Mpz_type : public Integer_type {
	static int _id;
public:
	Mpz_type();
	virtual ~Mpz_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif