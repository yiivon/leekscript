#ifndef REAL_TYPE_HPP
#define REAL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Real_type : public Base_type {
public:
	Real_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const { return "real"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif