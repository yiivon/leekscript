#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class String_type : public Base_type {
public:
	String_type() {}
	virtual const std::string getName() const { return "string"; }
	virtual const std::string getClass() const { return "String"; }
	virtual const std::string getJsonName() const { return "string"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif