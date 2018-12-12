#ifndef CLASS_TYPE_HPP
#define CLASS_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Class_type : public Pointer_type {
public:
	Class_type();
	virtual int id() const override { return 11; }
	virtual const std::string getName() const { return "class"; }
	virtual const std::string getJsonName() const { return "class"; }
	virtual bool operator == (const Base_type*) const override;
	virtual bool callable() const override { return true; }
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif