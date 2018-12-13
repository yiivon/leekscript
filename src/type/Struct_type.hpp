#ifndef STRUCT_TYPE_HPP
#define STRUCT_TYPE_HPP

#include "Base_type.hpp"
#include "llvm/IR/DerivedTypes.h"

namespace ls {

class Struct_type : public Base_type {
	std::string _name;
	std::vector<Type> _types;
	llvm::StructType* _llvm_type;
public:
	Struct_type(const std::string name, std::initializer_list<Type> types);
	virtual const std::string getName() const { return "struct"; }
	virtual const std::string getJsonName() const { return "struct"; }
	virtual Type member(int) const override;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif