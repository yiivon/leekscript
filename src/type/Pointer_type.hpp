#ifndef POINTER_TYPE_HPP
#define POINTER_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Pointer_type : public Type {
	const Type* const _type;
	llvm::Type* _llvm_type = nullptr;
public:
	Pointer_type(const Type* type, bool native = false);
	virtual ~Pointer_type() {}
	virtual const Type* pointed() const override;
	virtual const std::string getName() const { return "pointer"; }
	virtual const std::string getJsonName() const override { return _type->getJsonName() + "*"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif