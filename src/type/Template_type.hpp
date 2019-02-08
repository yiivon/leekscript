#ifndef TEMPLATE_TYPE_HPP
#define TEMPLATE_TYPE_HPP

#include "Any_type.hpp"
#include "Type.hpp"

namespace ls {

class Template_type : public Any_type {
	std::string _name;
public:
	Type _implementation;
	Template_type(const std::string name) : _name(name) {}
	void implement(Type type) const;
	virtual bool operator == (const Base_type*) const override;
	virtual bool compatible(const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	
};

}

#endif