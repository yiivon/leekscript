#ifndef OBJECT_TYPE_HPP
#define OBJECT_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Object_type : public Base_type {
public:
	Object_type() {}
	virtual const std::string getName() const { return "object"; }
	virtual const std::string getClass() const { return "Object"; }
	virtual const std::string getJsonName() const { return "object"; }
	virtual bool iterable() const { return false; } // TODO not iterable for now
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif