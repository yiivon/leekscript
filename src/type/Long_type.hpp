#ifndef LONG_TYPE_HPP
#define LONG_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Long_type : public Base_type {
public:
	Long_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const { return "long"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool iterable() const { return true; }
	virtual Type element() const override;
	virtual Type iterator() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif