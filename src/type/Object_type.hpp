#ifndef OBJECT_TYPE_HPP
#define OBJECT_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Object_type : public Base_type {
public:
	Object_type();
	virtual ~Object_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif