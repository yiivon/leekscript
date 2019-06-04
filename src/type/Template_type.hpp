#ifndef TEMPLATE_TYPE_HPP
#define TEMPLATE_TYPE_HPP

#include "Any_type.hpp"

namespace ls {

class Type;

class Template_type : public Any_type {
	std::string _name;
public:
	const Type* _implementation = Type::void_;
	Template_type(const std::string name) : _name(name) {}
	virtual void implement(const Type* type) const override;
	virtual bool operator == (const Type*) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
	
};

}

#endif