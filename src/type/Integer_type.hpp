#ifndef INTEGER_TYPE_HPP
#define INTEGER_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Integer_type : public Base_type {
public:
	Integer_type();
	virtual ~Integer_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif