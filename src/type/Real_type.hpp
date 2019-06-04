#ifndef REAL_TYPE_HPP
#define REAL_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Real_type : public Type {
public:
	Real_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const override { return "real"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif