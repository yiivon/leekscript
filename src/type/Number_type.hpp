#ifndef NUMBER_TYPE_HPP
#define NUMBER_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Number_type : public Base_type {
public:
	Number_type();
	virtual int id() const override { return 3; }
	virtual const std::string getName() const { return "number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool iterable() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif