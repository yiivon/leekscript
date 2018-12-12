#ifndef OBJECT_TYPE_HPP
#define OBJECT_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Object_type : public Pointer_type {
public:
	Object_type();
	virtual int id() const override { return 10; }
	virtual const std::string getName() const { return "object"; }
	virtual const std::string getJsonName() const { return "object"; }
	virtual bool iterable() const { return false; } // TODO not iterable for now
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif