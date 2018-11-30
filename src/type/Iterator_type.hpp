#ifndef ITERATOR_TYPE_HPP
#define ITERATOR_TYPE_HPP

#include "Type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class Iterator_type : public BaseRawType {
	Type _container;
public:
	Iterator_type(Type container) : _container(container) {}
	virtual int id() const { return 60; }
	virtual const std::string getName() const { return "iterator"; }
	virtual const std::string getClass() const { return "Iterator"; }
	virtual const std::string getJsonName() const { return "iterator"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual bool operator == (const BaseRawType*) const override;
	virtual bool compatible(const BaseRawType*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream&) const;
};

}

#endif