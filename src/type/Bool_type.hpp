#ifndef BOOL_TYPE_HPP
#define BOOL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Bool_type : public Base_type {
public:
	Bool_type() {}
	virtual int id() const override { return 2; }
	virtual const std::string getName() const { return "bool"; }
	virtual const std::string getJsonName() const { return "boolean"; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif