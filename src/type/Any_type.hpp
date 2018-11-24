#ifndef ANY_TYPE_HPP
#define ANY_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Any_type : public Base_type {
public:
	virtual ~Any_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif