#ifndef POINTER_TYPE_HPP
#define POINTER_TYPE_HPP

#include "Type.hpp"
#include "Base_type.hpp"

namespace ls {

class Pointer_type : public Base_type {
	Type _type;
	llvm::Type* _llvm_type = nullptr;
public:
	Pointer_type(Type type);
	Type pointed() const;
	virtual const std::string getName() const { return "pointer"; }
	virtual const std::string getJsonName() const { return _type.getJsonName() + "*"; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif