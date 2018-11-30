#ifndef ANY_TYPE_HPP
#define ANY_TYPE_HPP

#include "Base_type.hpp"
#include "Base_type.hpp"

namespace ls {

class Any_type : public Base_type {
public:
	Any_type() {}
	virtual ~Any_type() {}
	virtual const std::string getName() const { return "any"; }
	virtual llvm::Type* llvm() const;
	virtual std::ostream& print(std::ostream& os) const override;
};

class Placeholder_type : public Base_type {
public:
	std::string name;
	Placeholder_type(std::string name) : name(name) {}
	virtual ~Placeholder_type() {}
	virtual const std::string getName() const { return name; }
	virtual bool is_placeholder() const { return true; }
	virtual llvm::Type* llvm() const override { }
};

}

#endif