#ifndef SET_TYPE_HPP
#define SET_TYPE_HPP

#include "Type.hpp"
#include "Base_type.hpp"

namespace ls {

class Set_type : public Base_type {
	Type _element;
public:
	Set_type(Type element) : _element(element) {}
	virtual const std::string getName() const { return "set"; }
	virtual const std::string getClass() const { return "Set"; }
	virtual const std::string getJsonName() const { return "set"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual bool compatible(const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif