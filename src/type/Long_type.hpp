#ifndef LONG_TYPE_HPP
#define LONG_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Long_type : public Number_type {
public:
	Long_type() {}
	virtual const std::string getName() const { return "long"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual Type element() const override;
	virtual Type iterator() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif