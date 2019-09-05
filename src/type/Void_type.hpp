#ifndef VOID_TYPE_HPP
#define VOID_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Void_type : public Type {
public:
	virtual int id() const { return 0; }
	virtual const std::string getName() const override { return "void"; }
	virtual const std::string getJsonName() const { return "void"; }
	virtual const Type* element() const override { return Type::void_; }
	virtual const Type* key() const override { return Type::void_; }
	virtual const Type* return_type() const override { return Type::void_; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif