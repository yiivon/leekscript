#ifndef NULL_TYPE_HPP
#define NULL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Null_type : public Base_type {
public:
	Null_type();
	virtual ~Null_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif