#ifndef MPZ_TYPE_HPP
#define MPZ_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Mpz_type : public Type {
public:
	Mpz_type() {}
	virtual int id() const override { return 13; }
	virtual const std::string getName() const { return "mpz"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;

	static llvm::Type* mpz_type;
	static llvm::Type* get_mpz_type(const Compiler& c);
};

}

#endif