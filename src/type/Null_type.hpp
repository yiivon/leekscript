#ifndef NULL_TYPE_HPP
#define NULL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Null_type : public Base_type {
public:
	Null_type() {}
	virtual int id() const { return 1; }
	virtual const std::string getName() const { return "null"; }
	virtual const std::string getJsonName() const { return "null"; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif