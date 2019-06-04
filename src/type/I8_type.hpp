#ifndef I8_TYPE_HPP
#define I8_TYPE_HPP

#include "Type.hpp"

namespace ls {

class I8_type : public Type {
public:
	I8_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const override { return "i8"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool operator == (const Type*) const;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual Type* clone() const override;
};

}

#endif