#ifndef MPZ_TYPE_HPP
#define MPZ_TYPE_HPP

#include "Integer_type.hpp"

namespace ls {

class Mpz_type : public Integer_type {
public:
	Mpz_type() {}
	virtual const std::string getName() const { return "mpz"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif