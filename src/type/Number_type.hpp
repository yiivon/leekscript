#ifndef NUMBER_TYPE_HPP
#define NUMBER_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Number_type : public Base_type {
public:
	Number_type() {}
	virtual const std::string getName() const { return "number"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool iterable() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif