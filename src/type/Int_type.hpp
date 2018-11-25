#ifndef INT_TYPE_HPP
#define INT_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Int_type : public Base_type {
public:
	Int_type();
	virtual ~Int_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif