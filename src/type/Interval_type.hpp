#ifndef INTERVAL_TYPE_HPP
#define INTERVAL_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Interval_type : public Pointer_type {
public:
	Interval_type();
	virtual int id() const override { return 8; }
	virtual const std::string getName() const { return "interval"; }
	virtual const std::string getJsonName() const { return "interval"; }
	virtual bool iterable() const { return true; }
	virtual Type iterator() const override;
	virtual const Type& key() const override;
	virtual const Type& element() const override;
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif