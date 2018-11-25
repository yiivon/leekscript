#ifndef REAL_TYPE_HPP
#define REAL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Real_type : public Base_type {
public:
	Real_type();
	virtual ~Real_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif