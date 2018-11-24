#ifndef NUMBER_TYPE_HPP
#define NUMBER_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Number_type : public Base_type {
public:
	Number_type();
	virtual ~Number_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif