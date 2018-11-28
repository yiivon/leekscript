#ifndef CLASS_TYPE_HPP
#define CLASS_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Class_type : public Base_type {
public:
	Class_type();
	virtual ~Class_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif