#ifndef LONG_TYPE_HPP
#define LONG_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Long_type : public Type {
public:
	Long_type() {}
	virtual int id() const override { return 3; }
	virtual const std::string getName() const override { return "long"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool iterable() const { return true; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual const Type* iterator() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif