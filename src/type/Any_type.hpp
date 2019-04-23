#ifndef ANY_TYPE_HPP
#define ANY_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Any_type : public Pointer_type {
public:
	Any_type();
	virtual ~Any_type() {}
	virtual const std::string getName() const { return "any"; }
	virtual bool operator == (const Base_type*) const override;
	virtual bool callable() const override { return true; }
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif