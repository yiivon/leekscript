#ifndef CLASS_TYPE_HPP
#define CLASS_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Class_type : public Base_type {
public:
	Class_type() {}
	virtual const std::string getName() const { return "class"; }
	virtual const std::string getClass() const { return "Class"; }
	virtual const std::string getJsonName() const { return "class"; }
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif