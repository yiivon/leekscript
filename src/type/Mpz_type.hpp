#ifndef MPZ_TYPE_HPP
#define MPZ_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Mpz_type : public Base_type {
public:
	Mpz_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const { return "mpz"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;

	static llvm::Type* mpz_type;
	static llvm::Type* get_mpz_type();
};

}

#endif