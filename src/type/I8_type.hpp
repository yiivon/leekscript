#ifndef I8_TYPE_HPP
#define I8_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class I8_type : public Base_type {
public:
	I8_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const { return "i8"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool operator == (const Base_type*) const;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif