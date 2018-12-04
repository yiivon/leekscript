#ifndef PLACEHOLDER_TYPE_HPP
#define PLACEHOLDER_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Placeholder_type : public Base_type {
	std::string _name;
public:
	Placeholder_type(const std::string name) : _name(name) {}
	virtual const std::string getName() const { return "object"; }
	virtual const std::string getJsonName() const { return "object"; }
	virtual bool iterable() const { return false; } // TODO not iterable for now
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif