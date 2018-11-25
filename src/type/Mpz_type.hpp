#ifndef MPZ_TYPE_HPP
#define MPZ_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Mpz_type : public Base_type {
public:
	Mpz_type();
	virtual ~Mpz_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif