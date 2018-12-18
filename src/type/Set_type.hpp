#ifndef SET_TYPE_HPP
#define SET_TYPE_HPP

#include "Type.hpp"
#include "Pointer_type.hpp"

namespace ls {

class Set_type : public Pointer_type {
	Type _element;
public:
	Set_type(Type element);
	virtual int id() const override { return 7; }
	virtual const std::string getName() const { return "set"; }
	virtual const std::string getJsonName() const { return "set"; }
	virtual bool iterable() const { return true; }
	virtual Type iterator() const override;
	virtual bool is_container() const { return true; }
	virtual Type key() const override;
	virtual Type element() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual bool compatible(const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif