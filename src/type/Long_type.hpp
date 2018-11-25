#ifndef LONG_TYPE_HPP
#define LONG_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Long_type : public Base_type {
public:
	Long_type();
	virtual ~Long_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif