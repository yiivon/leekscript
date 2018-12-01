#ifndef ITERATOR_TYPE_HPP
#define ITERATOR_TYPE_HPP

#include "Type.hpp"
#include "Base_type.hpp"

namespace ls {

class Iterator_type : public Base_type {
	Type _container;
public:
	Iterator_type(Type container) : _container(container) {}
	virtual int id() const { return 60; }
	virtual const std::string getName() const { return "iterator"; }
	virtual const std::string getJsonName() const { return "iterator"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual bool compatible(const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream&) const;
};

}

#endif